# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

qt_policy(SET QTP0001 NEW)

qt_add_executable(pointless main.cpp controller.cpp)

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
    LaterView.qml)

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
  controller.h)

target_link_libraries(pointless PRIVATE Qt6::Core Qt6::Gui Qt6::Quick pointless_core)
