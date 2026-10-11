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
