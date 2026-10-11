# Port order — pointless_cpp

Tier N = max(tier of its dependencies) + 1. Tier 0 is the framework: those types are not ported, they are mapped to Slint equivalents. Everything from Tier 1 up is project code to port, and a tier only depends on lower tiers, so tiers can be done in order and the types within one tier are independent of each other.

## Tier 0

Framework types — map, don't port.

- **QtQml** — `Connections` (2)
- **QtQuick** — `Behavior` (6), `ColorAnimation` (1), `Flow` (1), `FocusScope` (2), `FontLoader` (2), `GridView` (1), `Item` (30), `ListView` (4), `MouseArea` (26), `NumberAnimation` (5), `Rectangle` (49), `Repeater` (3), `RotationAnimation` (3), `Row` (2), `TapHandler` (1), `Text` (69)
- **QtQuick.Controls** — `ApplicationWindow` (1), `Button` (10), `CheckBox` (1), `ScrollBar` (2), `ScrollView` (1), `TextArea` (1), `TextField` (6)
- **QtQuick.Layouts** — `ColumnLayout` (20), `RowLayout` (21)

## Tier 1

11 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `CalendarModel` | C++ | `src/gui/calendarmodel.h` + `src/gui/calendarmodel.cpp` | 128 | — |
| `CalendarsModel` | C++ | `src/gui/calendarsmodel.h` + `src/gui/calendarsmodel.cpp` | 176 | — |
| `ErrorController` | C++ | `src/gui/error_controller.h` + `src/gui/error_controller.cpp` | 71 | — |
| `GuiController` (singleton) | C++ | `src/gui/gui_controller.h` + `src/gui/gui_controller.cpp` | 1777 | — |
| `PomodoroController` | C++ | `src/gui/pomodoro_controller.h` + `src/gui/pomodoro_controller.cpp` | 141 | — |
| `TagFilterModel` | C++ | `src/gui/tagfiltermodel.h` + `src/gui/tagfiltermodel.cpp` | 101 | — |
| `TagModel` | C++ | `src/gui/tagmodel.h` + `src/gui/tagmodel.cpp` | 135 | — |
| `TaskFilterModel` | C++ | `src/gui/taskfiltermodel.h` + `src/gui/taskfiltermodel.cpp` | 351 | — |
| `TaskModel` | C++ | `src/gui/taskmodel.h` + `src/gui/taskmodel.cpp` | 397 | — |
| `WeekdayFilterModel` | C++ | `src/gui/weekdayfiltermodel.h` + `src/gui/weekdayfiltermodel.cpp` | 148 | — |
| `WeekdayModel` | C++ | `src/gui/weekdaymodel.h` + `src/gui/weekdaymodel.cpp` | 155 | — |

## Tier 2

1 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `Style` (singleton) | QML | `src/gui/Style.qml` | 65 | `GuiController` |

## Tier 3

13 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `AddTagPopup` | QML | `src/gui/AddTagPopup.qml` | 132 | `GuiController`, `Style` |
| `CheckBox` | QML | `src/gui/CheckBox.qml` | 31 | `Style` |
| `ErrorPopup` | QML | `src/gui/ErrorPopup.qml` | 83 | `GuiController`, `Style` |
| `FontAwesomeButton` | QML | `src/gui/FontAwesomeButton.qml` | 36 | `Style` |
| `LineEdit` | QML | `src/gui/LineEdit.qml` | 52 | `Style` |
| `Menu` | QML | `src/gui/Menu.qml` | 25 | `Style` |
| `MenuItem` | QML | `src/gui/MenuItem.qml` | 37 | `Style` |
| `NotesPopup` | QML | `src/gui/NotesPopup.qml` | 145 | `GuiController`, `Style` |
| `RenameTagPopup` | QML | `src/gui/RenameTagPopup.qml` | 139 | `GuiController`, `Style` |
| `SidePanel` | QML | `src/gui/SidePanel.qml` | 229 | `GuiController`, `Style` |
| `Tag` | QML | `src/gui/Tag.qml` | 44 | `GuiController`, `Style` |
| `ToggleButton` | QML | `src/gui/ToggleButton.qml` | 50 | `Style` |
| `ViewButton` | QML | `src/gui/ViewButton.qml` | 29 | `Style` |

## Tier 4

6 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `Calendar` | QML | `src/gui/Calendar.qml` | 153 | `CalendarModel`, `FontAwesomeButton`, `Style` |
| `LoginScreen` | QML | `src/gui/LoginScreen.qml` | 256 | `FontAwesomeButton`, `GuiController`, `Style` |
| `Page` | QML | `src/gui/Page.qml` | 69 | `FontAwesomeButton`, `GuiController`, `Style` |
| `TagsView` | QML | `src/gui/TagsView.qml` | 75 | `FontAwesomeButton`, `GuiController`, `Style`, `Tag` |
| `Task` | QML | `src/gui/Task.qml` | 187 | `CheckBox`, `FontAwesomeButton`, `GuiController`, `Style`, `Tag` |
| `WeekNavigator` | QML | `src/gui/WeekNavigator.qml` | 55 | `FontAwesomeButton`, `GuiController`, `Style` |

## Tier 5

7 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `AboutPage` | QML | `src/gui/AboutPage.qml` | 65 | `GuiController`, `Page`, `Style` |
| `CalendarsPage` | QML | `src/gui/CalendarsPage.qml` | 170 | `CheckBox`, `FontAwesomeButton`, `GuiController`, `Page`, `Style` |
| `CleanupPage` | QML | `src/gui/CleanupPage.qml` | 67 | `GuiController`, `Page`, `Style` |
| `EditTaskPage` | QML | `src/gui/EditTaskPage.qml` | 192 | `Calendar`, `CheckBox`, `GuiController`, `LineEdit`, `Page`, `Style`, `Tag` |
| `TagsPage` | QML | `src/gui/TagsPage.qml` | 27 | `AddTagPopup`, `Page`, `RenameTagPopup`, `TagsView` |
| `TaskView` | QML | `src/gui/TaskView.qml` | 87 | `GuiController`, `Style`, `Task`, `TaskFilterModel` |
| `WeekView` | QML | `src/gui/WeekView.qml` | 159 | `FontAwesomeButton`, `GuiController`, `Style`, `Task`, `TaskFilterModel`, `WeekdayFilterModel`, `WeekdayModel` |

## Tier 6

5 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `GoalsView` | QML | `src/gui/GoalsView.qml` | 6 | `TaskView` |
| `LaterView` | QML | `src/gui/LaterView.qml` | 7 | `TaskView` |
| `SearchPage` | QML | `src/gui/SearchPage.qml` | 67 | `Page`, `Style`, `TaskView` |
| `SoonView` | QML | `src/gui/SoonView.qml` | 6 | `TaskView` |
| `TasksByTagPage` | QML | `src/gui/TasksByTagPage.qml` | 23 | `GuiController`, `Page`, `TaskView` |

## Tier 7

1 type(s) to port.

| Type | Kind | File | LOC | Depends on |
|---|---|---|---|---|
| `Main` | QML | `src/gui/Main.qml` | 439 | `AboutPage`, `CalendarsPage`, `CleanupPage`, `EditTaskPage`, `ErrorPopup`, `FontAwesomeButton`, `GoalsView`, `GuiController`, `LaterView`, `LoginScreen`, `Menu`, `MenuItem`, `NotesPopup`, `SearchPage`, `SidePanel`, `SoonView`, `Style`, `TagsPage`, `TasksByTagPage`, `ToggleButton`, `ViewButton`, `WeekNavigator`, `WeekView` |

## C++ registration review

**5 type(s) QML never instantiates** but does reach, so the registration is needed - only its creatability is not. `QML_ELEMENT` alone lets QML construct them; adding `QML_UNCREATABLE` states that they are obtained from C++, which is what the port has to reproduce.

| Type | Header | How QML reaches it |
|---|---|---|
| `CalendarsModel` | `src/gui/calendarsmodel.h` | `GuiController.calendarsModel` (property) |
| `ErrorController` | `src/gui/error_controller.h` | `GuiController.errorController` (property) |
| `TagFilterModel` | `src/gui/tagfiltermodel.h` | `GuiController.tagFilterModel` (property) |
| `TagModel` | `src/gui/tagmodel.h` | `GuiController.tagModel` (property) |
| `TaskModel` | `src/gui/taskmodel.h` | `GuiController.taskModel` (property) |

`PomodoroController` in this module already does this, so the change is consistent with existing code.
