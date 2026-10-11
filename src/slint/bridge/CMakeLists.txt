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
find_package(Slint REQUIRED)

add_subdirectory(${QT2SLINT_DIR}/bridge/lib bridge_lib)
include(${QT2SLINT_DIR}/bridge/cmake/QtSlintBridgeBindings.cmake)

qtslintbridge_generate_bindings(pointless_bindings CONFIG bridge.toml QOBJECT_TARGETS pointless_gui)
get_target_property(BINDINGS_DIR pointless_bindings QTSLINTBRIDGE_BINDINGS_DIR)

add_executable(pointless_slint main.cpp)
slint_target_sources(pointless_slint slint/app.slint NAMESPACE ui LIBRARY_PATHS bridge=${BINDINGS_DIR})
target_include_directories(pointless_slint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../gui)
target_link_libraries(
  pointless_slint
  PRIVATE pointless_bindings
          pointless_gui
          pointless_core
          QtSlintBridge::Bridge
          Qt6::Core
          Qt6::Qml
          Qt6::Concurrent
          Qt6::Widgets
          Slint::Slint)
