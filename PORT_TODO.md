# PORT_TODO

Qt/QML → Slint port tasks (see /pub_data/etc/ai/qt_to_slint/PORT.md).

- [x] 1. Build the Qt project
  - [x] 1.1 Run `ninja all_qmllint` — no errors allowed
- [x] 2. Run `tools/qml_inventory.py` to create `inventory.json`
- [x] 3. Run `tools/qml_inventory.py --ordered` to create `TODO_TYPES.md`
- [x] 4. Match Tier 0 built-ins against `MAP.md`, add the missing ones
- [x] 1.2 Add `POINTLESS_SLINT_OVER_QTCORE` option (defines `SLINT_OVER_CORE`) and the `dev-slint` preset
- [x] 5. Slint submodule in `3rdparty/slint`, `scripts/build_slint.sh` (fixed its paths after the move to `scripts/`)
- [ ] 6. Bridge glue and cmake
  - [x] QApplication under `SLINT_OVER_CORE`, link Qt6::Widgets
  - [x] Dummy `src/slint/bridge/slint/app.slint`
  - [x] Bridge glue in `src/slint/bridge/` (needs `QT2SLINT_DIR` to point to the qt2slint checkout)
  - [ ] Blockers: `GuiController::passStoreCredentialsFetched(QVariantMap)` has runtime keys (`caldav-url-N`, `ical-url-N`...), unsupported by the bridge, the signal is skipped
  - [x] Build `dev-slint` (all targets build, `pointless_slint` starts offscreen)
  - [ ] Commit
- [x] 7. Take screenshots of the existing Qt app
  - [x] `screenshot_collection` Spix test (`src/gui/tests/screenshot_collection.cpp`), run it with `QT_QPA_PLATFORM=offscreen build-dev/bin/screenshot_collection` (disabled in ctest as it writes into the source tree)
  - [x] Added objectNames to `SidePanel.qml`, `Page.qml` and the pages in `Main.qml`, so Spix can reach the side panel entries and back buttons
  - Not captured: login screen (needs unauthenticated state), error popup, add tag / rename tag popups, tasks by tag page, the debug menu entry
  - Note: the date picker in the edit task page ignores the test clock and shows the real current month, so that screenshot changes over time

- [x] 8. The actual port (`.qml` → `.slint` in `src/slint/bridge/slint/`, builds and starts offscreen, not looked at yet: that's step 9)
  - Qt-side changes the port needed, as Slint can't do them: `CalendarModel` got `monthTitle` and `showMonthOf()`/`previousMonth()`/`nextMonth()` (no date parsing or formatting in Slint), `GuiController::colorFromTag()` returns a `QColor` (no string to color in Slint), new `GuiController::otherTags()` (no string split)
  - Not ported 1:1: text isn't trimmed (tag names in the add/rename popups, the task title in the edit page, Slint has no trim), the color dot in the calendars page (the QML Rectangle has no size, never showed), `Connections.onIsEditingTaskChanged` (focus handling), `Qt.inputMethod` in the notes popup, `GuiController.passStoreCredentialsFetched` / `pomodoroFinished` (nothing in the QML uses them)
  - Strikeout of done tasks is a 1px line over the title (Slint `Text` has no strikeout)
- [x] 9. Verification (Slint screenshots)
  - `screenshot_collection_slint` (`src/slint/bridge/`) runs the same test as the Qt one (`src/gui/tests/screenshot_collection.cpp`) through a Slint harness (`gui_test_harness_slint.cpp`), run it with `QT_QPA_PLATFORM=offscreen build-dev-slint/bin/screenshot_collection_slint`. Images go to `screenshots/slint/`, compare them with `screenshots/qt/`
  - Spix comes from the qt2slint checkout (`$QT2SLINT_DIR/3rdparty/spix`, it has the Slint scene) in the `dev-slint` preset, `3rdparty/spix` has none. Its Slint scene needs `stb_image_write.h`: configure with `-DSTB_INCLUDE_DIR=<dir with stb_image_write.h>` until `stb` is in `vcpkg.json`
  - The two UIs match closely on all 14 screens, not pixel perfect

# GUI tests

- `test_gui` runs against both UIs (`test_gui` in the `dev` preset, `test_gui_slint` in `dev-slint`, where `test_gui` is disabled). It reads the same properties in both: `checked`, `count`, `text`, `existsAndVisible()`. See `TODO.md` of qt2slint for what Spix can't do for Slint

# Improvements (10)

Done:
- Cancel/Save buttons are sized by their text, as in Qt
- Tag names are trimmed in `GuiController::addTag()`/`renameTag()` (Slint can't trim), the popups' text fields are centered
- The screenshot test also captures the add tag popup, the rename tag popup and the tasks by tag page (new objectNames in `TagsPage.qml`, `TagsView.qml` and the popups)

Not a bug of the port:
- Later view: Later Task 16 and 17 are swapped compared with Qt. `TaskFilterModel::lessThan()` isn't a strict weak ordering (an undated task compares by tag and row against a dated one), so the order depends on the sort algorithm's input. The Slint order follows the comparator, the Qt one is by chance. Decide what the order should be, then fix the comparator
- No text cursor in the Slint screenshots: the offscreen window isn't active. Not seen in a real window
- Slint (Fluent style) always shows a thin scroll bar when the content is larger than the view, Qt hides it until used

Left:
- Not captured by the screenshot test (as for Qt): login screen, error popup
- The edit task date picker shows the test clock's month in Slint and the real month in Qt (a bug of the Qt version)
- Task titles are not trimmed on save, as in the QML

# GUI polish (11)

Done:
- Thin text with the Qt backend: with a variable system font (Noto Sans on the other computer) the regular text was drawn with the lightest weight, as `QRawFont::loadFromData()` ignores the default axes. Fixed in the Slint fork (`3rdparty/slint`, branch `qt-font-hinting`). Reproduced here with Ubuntu Sans. The femtovg backend wasn't affected
- Icons with an outline with the Qt backend (Arch, scaled screen): glyphs of 64 pixels or more are drawn as a path, with the pen left by the previous drawing. Fixed in the Slint fork (`qt-font-hinting`). Reproduced here with a bigger `Style.scale-factor`
- Text: the Slint Qt backend disabled font hinting, which made text thinner and blurrier than Qt's. `3rdparty/slint` (branch `qt-font-hinting` of the fork, to be pushed) uses the system's hinting, `SLINT_QT_FONT_HINTING=default|none|vertical|full` overrides it. The offscreen screenshots don't show a difference (no fontconfig hint settings there), check it in a real window
- The task list has no scroll bar (Fluent's is a permanent line over the tasks, Qt's only shows when used), so it can't be dragged, wheel and touch scroll
- The checkbox of a task is as wide as in Qt, so the title lines up
- The Qt screenshot of the tasks by tag page was the tags page (the tap handler of the row ignores the test's click), now the test clicks the tag

Too hard or left:
- Dragging a scroll bar (a custom Slint scroll bar would be needed)
- The `Later` order and the other items of step 10
