# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2)

qt_policy(SET QTP0001 NEW)

qt_add_executable(pointless main.cpp controller.cpp taskmodel.cpp tagmodel.cpp)

set_target_properties(pointless PROPERTIES AUTOMOC ON AUTORCC ON)

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
    CheckBox.qml)

qt_add_qml_module(
  pointless
  URI
  pointless
  VERSION
  1.0
  QML_FILES
  ${PLAIN_QML_FILES}
  ${QML_SINGLETONS}
  SOURCES
  controller.cpp
  controller.h
  taskmodel.cpp
  taskmodel.h
  tagmodel.cpp
  tagmodel.h
  taskfiltermodel.cpp
  RESOURCES
  fonts/FontAwesome7Brands-Regular-400.otf
  fonts/FontAwesome7Free-Regular-400.otf
  fonts/FontAwesome7-Solid-900.otf)

target_link_libraries(pointless PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::QuickControls2 pointless_core)

if(IOS)
  set_target_properties(
    pointless
    PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${BUNDLE_IDENTIFIER}
               MACOSX_BUNDLE_IDENTIFIER ${BUNDLE_IDENTIFIER}
               MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Info.plist")
endif()
