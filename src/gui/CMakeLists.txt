# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Concurrent)

qt_policy(SET QTP0001 NEW)

qt_add_library(pointless_gui STATIC)

if(POINTLESS_ENABLE_TESTS)
  target_link_libraries(pointless_gui PRIVATE Spix::QtQuick)
endif()

set_target_properties(pointless_gui PROPERTIES AUTOMOC ON AUTORCC ON)

target_include_directories(pointless_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)

set(QML_SINGLETONS Style.qml)

set_source_files_properties(${QML_SINGLETONS} PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

set(PLAIN_QML_FILES
    Main.qml
    WeekNavigator.qml
    Task.qml
    Tag.qml
    WeekView.qml
    SoonView.qml
    LaterView.qml
    TaskView.qml
    FontAwesomeButton.qml
    ViewButton.qml
    LoginScreen.qml
    CheckBox.qml
    Calendar.qml
    LineEdit.qml
    EditTask.qml
    Menu.qml
    MenuItem.qml)

qt_add_qml_module(
  pointless_gui
  URI
  pointless
  VERSION
  1.0
  QML_FILES
  ${PLAIN_QML_FILES}
  ${QML_SINGLETONS}
  SOURCES
  application.cpp
  Clock.cpp
  gui_controller.cpp
  data_controller.cpp
  error_controller.cpp
  date_utils.cpp
  local_settings.cpp
  utils.cpp
  gui_controller.h
  local_settings.h
  utils.h
  taskmodel.cpp
  taskmodel.h
  tagmodel.cpp
  tagmodel.h
  taskfiltermodel.cpp
  taskfiltermodel.h
  tagfiltermodel.cpp
  tagfiltermodel.h
  weekdayfiltermodel.cpp
  weekdayfiltermodel.h
  weekdaymodel.cpp
  weekdaymodel.h
  calendarmodel.cpp
  calendarmodel.h
  RESOURCES
  fonts/FontAwesome7Brands-Regular-400.otf
  fonts/FontAwesome7Free-Regular-400.otf
  fonts/FontAwesome7-Solid-900.otf
  OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/pointless
  # NO_PLUGIN
)

target_link_libraries(pointless_gui PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::QuickControls2 Qt6::Concurrent pointless_core)
target_link_libraries(pointless_gui PRIVATE pointless_guiplugin)

qt_add_executable(pointless main.cpp)
target_link_libraries(pointless PRIVATE pointless_gui Qt6::Core Qt6::Gui Qt6::Quick)

if(IOS)
  if(NOT POINTLESS_DEVELOPER_MODE)
    # set release provisioning stuff

    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set(CMAKE_XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER "")

    ios_set_manual_signing_properties(pointless)
  endif()

  set_target_properties(
    pointless
    PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${BUNDLE_IDENTIFIER}
               MACOSX_BUNDLE_IDENTIFIER ${BUNDLE_IDENTIFIER}
               MACOSX_BUNDLE TRUE
               XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
               MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Info.plist")

  target_sources(pointless PRIVATE ${CMAKE_SOURCE_DIR}/assets/ios/Images.xcassets)
  set_source_files_properties("${CMAKE_SOURCE_DIR}/assets/ios/Images.xcassets" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if(POINTLESS_ENABLE_TESTS)
  add_executable(test_gui tests/test_gui.cpp)
  target_link_libraries(test_gui PRIVATE pointless_core pointless_gui GTest::gtest_main Spix::QtQuick)
  target_include_directories(test_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_definitions(test_gui PRIVATE POINTLESS_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
  add_test(NAME test_gui COMMAND test_gui --local)
  # add_test(NAME test_gui_supabase COMMAND test_gui --supabase)

  add_executable(test_data_controller tests/test_data_controller.cpp)
  target_link_libraries(test_data_controller PRIVATE pointless_core pointless_gui GTest::gtest_main Qt6::Core Qt6::Quick Qt6::Test Qt6::Concurrent)
  target_include_directories(test_data_controller PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_data_controller COMMAND test_data_controller)

  add_executable(test_gui_date_utils tests/test_date_utils.cpp)
  target_link_libraries(test_gui_date_utils PRIVATE pointless_core pointless_gui GTest::gtest_main Qt6::Core)
  target_include_directories(test_gui_date_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_gui_date_utils COMMAND test_gui_date_utils)

  add_executable(test_calendarmodel tests/test_calendarmodel.cpp)
  target_link_libraries(test_calendarmodel PRIVATE pointless_core pointless_gui GTest::gtest_main Qt6::Core)
  target_include_directories(test_calendarmodel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_calendarmodel COMMAND test_calendarmodel)

  add_executable(test_local_settings tests/test_local_settings.cpp)
  target_link_libraries(test_local_settings PRIVATE pointless_core pointless_gui GTest::gtest_main Qt6::Core)
  target_include_directories(test_local_settings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_local_settings COMMAND test_local_settings)
endif()
