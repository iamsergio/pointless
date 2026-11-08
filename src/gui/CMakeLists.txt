# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Concurrent)
if(POINTLESS_SLINT_OVER_QTCORE)
  find_package(Qt6 REQUIRED COMPONENTS Widgets)
endif()

qt_policy(SET QTP0001 NEW)

qt_add_library(pointless_gui STATIC)

if(POINTLESS_ENABLE_TESTS)
  target_link_libraries(pointless_gui PRIVATE Spix::QtQuick)
endif()

set_target_properties(pointless_gui PROPERTIES AUTOMOC ON AUTORCC ON)

target_include_directories(pointless_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)

set(QML_SINGLETONS Style.qml)

set_source_files_properties(${QML_SINGLETONS} PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

if(POINTLESS_DEVELOPER_MODE)
  target_compile_definitions(pointless_gui PRIVATE QT_QML_DEBUG)
endif()

if(POINTLESS_YOCTO_BUILD)
  target_compile_definitions(pointless_gui PRIVATE POINTLESS_OS_YOCTO)
endif()

set(PLAIN_QML_FILES
    Main.qml
    MainView.qml
    WeekNavigator.qml
    WeekView.qml
    SoonView.qml
    LaterView.qml
    ViewButton.qml
    IconButton.qml
    TaskRow.qml
    TaskList.qml
    LoginScreen.qml
    Calendar.qml
    Page.qml
    EditTaskPage.qml
    AddTagPopup.qml
    RenameTagPopup.qml
    AboutPage.qml
    TagsPage.qml
    CalendarsPage.qml
    CleanupPage.qml
    ErrorPopup.qml
    NotesPopup.qml
    Menu.qml
    MenuItem.qml
    SidePanel.qml
    Switch.qml
    TagsView.qml
    TasksByTagPage.qml
    SearchPage.qml
    GoalsView.qml)

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
  pointless_init.cpp
  pointless_settings.cpp
  Clock.cpp
  gui_controller.cpp
  data_controller.cpp
  error_controller.cpp
  login_screen_controller.cpp
  pomodoro_controller.cpp
  date_utils.cpp
  local_settings.cpp
  utils.cpp
  taskmodel.cpp
  tagmodel.cpp
  taskfiltermodel.cpp
  tagfiltermodel.cpp
  weekdayfiltermodel.cpp
  weekdaymodel.cpp
  calendarmodel.cpp
  calendarsmodel.cpp
  RESOURCES
  fonts/FontAwesome7Brands-Regular-400.otf
  fonts/FontAwesome7Free-Regular-400.otf
  fonts/FontAwesome7-Solid-900.otf
  OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/pointless
  # NO_PLUGIN
)

target_link_libraries(pointless_gui PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::QuickControls2 Qt6::Concurrent pointless_core)
target_link_libraries(pointless_gui PRIVATE pointless_guiplugin)
if(POINTLESS_SLINT_OVER_QTCORE)
  # Application is a QApplication, which Slint's Qt backend needs
  target_link_libraries(pointless_gui PUBLIC Qt6::Widgets)
endif()

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
               MACOSX_BUNDLE_INFO_PLIST "${PROJECT_BINARY_DIR}/Info.plist")

  target_sources(pointless PRIVATE ${PROJECT_SOURCE_DIR}/assets/ios/Images.xcassets)
  set_source_files_properties("${PROJECT_SOURCE_DIR}/assets/ios/Images.xcassets" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

elseif(APPLE) # macOS
  set_target_properties(
    pointless
    PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${BUNDLE_IDENTIFIER}
               MACOSX_BUNDLE_IDENTIFIER ${BUNDLE_IDENTIFIER}
               MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_INFO_PLIST "${PROJECT_BINARY_DIR}/Info.plist")
endif()

if(POINTLESS_ENABLE_TESTS)
  add_library(gui_test_harness_qt STATIC tests/gui_test_harness_qt.cpp)
  target_link_libraries(
    gui_test_harness_qt
    PUBLIC pointless_core Spix::Core
    PRIVATE pointless_gui Spix::QtQuick Qt6::Core Qt6::Gui Qt6::Quick)
  target_include_directories(gui_test_harness_qt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  target_include_directories(gui_test_harness_qt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  add_executable(test_gui tests/test_gui.cpp)
  target_link_libraries(test_gui PRIVATE pointless_core gui_test_harness_qt Spix::Core GTest::gtest_main)
  target_compile_definitions(test_gui PRIVATE POINTLESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
  add_test(NAME test_gui COMMAND test_gui --local)
  if(POINTLESS_SLINT_OVER_QTCORE)
    # It drives the QML UI, which isn't built in this preset, test_gui_slint is the Slint version
    set_tests_properties(test_gui PROPERTIES DISABLED TRUE)
  endif()
  # add_test(NAME test_gui_supabase COMMAND test_gui --supabase)

  add_executable(screenshot_collection tests/screenshot_collection.cpp)
  target_link_libraries(screenshot_collection PRIVATE pointless_core gui_test_harness_qt Spix::Core GTest::gtest_main)
  target_compile_definitions(screenshot_collection PRIVATE POINTLESS_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
  # Writes into screenshots/, so not part of the default ctest run
  add_test(NAME screenshot_collection COMMAND screenshot_collection)
  set_tests_properties(screenshot_collection PROPERTIES DISABLED TRUE)

  add_executable(test_data_controller tests/test_data_controller.cpp)
  target_link_libraries(
    test_data_controller
    PRIVATE pointless_core
            pointless_gui
            GTest::gtest_main
            Qt6::Core
            Qt6::Quick
            Qt6::Test
            Qt6::Concurrent)
  target_include_directories(test_data_controller PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_data_controller COMMAND test_data_controller)

  add_executable(test_gui_controller tests/test_gui_controller.cpp)
  target_link_libraries(
    test_gui_controller
    PRIVATE pointless_core
            pointless_gui
            GTest::gtest_main
            Qt6::Core
            Qt6::Quick
            Qt6::Test
            Qt6::Concurrent)
  target_include_directories(test_gui_controller PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_gui_controller COMMAND test_gui_controller)

  add_executable(test_cli tests/test_cli.cpp)
  target_link_libraries(test_cli PRIVATE pointless_core GTest::gtest_main Qt6::Core)
  target_compile_definitions(test_cli PRIVATE POINTLESS_EXECUTABLE="$<TARGET_FILE:pointless>")
  add_dependencies(test_cli pointless)
  add_test(NAME test_cli COMMAND test_cli)

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
