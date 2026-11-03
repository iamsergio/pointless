# SPDX-FileCopyrightText: 2026 Sergio Martins
#
# SPDX-License-Identifier: MIT

# The QtCore to Slint bridge is developed in the qt2slint repo. The QT2SLINT_DIR environment variable must point to it.
if(NOT DEFINED ENV{QT2SLINT_DIR})
  message(FATAL_ERROR "POINTLESS_SLINT_OVER_QTCORE needs the QT2SLINT_DIR environment variable, the qt2slint checkout")
endif()
set(QT2SLINT_DIR "$ENV{QT2SLINT_DIR}")
if(NOT EXISTS "${QT2SLINT_DIR}/bridge/lib/CMakeLists.txt")
  message(FATAL_ERROR "QT2SLINT_DIR is not a qt2slint checkout: ${QT2SLINT_DIR}/bridge/lib/CMakeLists.txt not found")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Concurrent)
if(NOT TARGET Slint::Slint) # the top-level CMakeLists.txt finds it for Spix
  find_package(Slint REQUIRED)
endif()

add_subdirectory(${QT2SLINT_DIR}/bridge/lib bridge_lib)
include(${QT2SLINT_DIR}/bridge/cmake/QtSlintBridgeBindings.cmake)

qtslintbridge_generate_bindings(pointless_bindings CONFIG bridge.toml QOBJECT_TARGETS pointless_gui)
get_target_property(BINDINGS_DIR pointless_bindings QTSLINTBRIDGE_BINDINGS_DIR)

# The compiled UI, shared by the app and the GUI tests
add_library(pointless_slint_ui STATIC)
slint_target_sources(pointless_slint_ui slint/app.slint NAMESPACE ui LIBRARY_PATHS bridge=${BINDINGS_DIR})
target_include_directories(pointless_slint_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../gui)
target_link_libraries(
  pointless_slint_ui
  PUBLIC pointless_bindings
         pointless_gui
         pointless_core
         QtSlintBridge::Bridge
         Qt6::Core
         Qt6::Qml
         Qt6::Concurrent
         Qt6::Widgets
         Slint::Slint)

add_executable(pointless_slint main.cpp)
target_include_directories(pointless_slint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../gui)
target_link_libraries(
  pointless_slint
  PRIVATE pointless_slint_ui
          pointless_bindings
          pointless_gui
          pointless_core
          QtSlintBridge::Bridge
          Qt6::Core
          Qt6::Qml
          Qt6::Concurrent
          Qt6::Widgets
          Slint::Slint)

if(POINTLESS_ENABLE_TESTS)
  # The screenshot collection test of the Qt UI, run against the Slint UI. The harness is the only part that differs.
  add_executable(screenshot_collection_slint gui_test_harness_slint.cpp ../../gui/tests/screenshot_collection.cpp)
  target_include_directories(screenshot_collection_slint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../gui ${CMAKE_CURRENT_SOURCE_DIR}/../../gui/tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
  target_compile_definitions(screenshot_collection_slint PRIVATE POINTLESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

  # test_gui, against the Slint UI
  add_executable(test_gui_slint gui_test_harness_slint.cpp ../../gui/tests/test_gui.cpp)
  target_include_directories(test_gui_slint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../gui ${CMAKE_CURRENT_SOURCE_DIR}/../../gui/tests ${CMAKE_CURRENT_SOURCE_DIR}/../..)
  target_compile_definitions(test_gui_slint PRIVATE POINTLESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
  target_link_libraries(test_gui_slint PRIVATE pointless_slint_ui Spix::Slint Spix::Core GTest::gtest_main)
  add_test(NAME test_gui_slint COMMAND test_gui_slint --local)
  set_tests_properties(test_gui_slint PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 120)
  target_link_libraries(
    screenshot_collection_slint
    PRIVATE pointless_slint_ui
            pointless_bindings
            pointless_gui
            pointless_core
            QtSlintBridge::Bridge
            Spix::Slint
            Spix::Core
            GTest::gtest_main
            Qt6::Core
            Qt6::Qml
            Qt6::Concurrent
            Qt6::Widgets
            Slint::Slint)
endif()
