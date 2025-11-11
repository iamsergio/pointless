# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

qt_policy(SET QTP0001 NEW)

qt_add_executable(pointless main.cpp controller.cpp)

set_target_properties(pointless PROPERTIES AUTOMOC ON AUTORCC ON)

set(qml_singletons Style.qml)

set_source_files_properties(${qml_singletons} PROPERTIES QT_QML_SINGLETON_TYPE
                                                         TRUE)

set(plain_qml_files
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
  ${plain_qml_files}
  ${qml_singletons}
  SOURCES
  controller.cpp
  controller.h)

target_link_libraries(pointless PRIVATE Qt6::Core Qt6::Gui Qt6::Quick
                                        pointless_core)
