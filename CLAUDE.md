# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Pointless is a Qt6/QML cross-platform to-do manager written in C++23 with optional Supabase cloud sync. Targets macOS, iOS, Linux, and Android.

## Build Commands

```bash
# Configure (debug)
cmake --preset=dev

# Build
cmake --build build-dev

# Run tests
ctest --test-dir build-dev --output-on-failure --verbose

# Run qmllint
ninja all_qmllint

# Other presets: rel, dev-asan, dev-tsan, ios-dev, ios-release, clang-tidy
```

**Required environment variables:**
- `POINTLESS_SUPABASE_URL` - Backend URL
- `POINTLESS_SUPABASE_ANON_KEY` - API key
- `POINTLESS_DEBUG_USERNAME` / `POINTLESS_DEBUG_PASSWORD` - Test account (for tests)

## Architecture

### Core Layer (`src/core/`)
Static library `pointless_core` - business logic and data:
- `Task`, `Tag`, `Data` - Core data structures with UUID-based identification
- `LocalData` - JSON file persistence using glaze library
- `SupabaseProvider` - Cloud sync with compression/base64 encoding
- `IDataProvider` - Abstract interface for data sources
- `AppleCalendarProvider` - EventKit integration (macOS/iOS)
- `Merger` - Conflict resolution for sync

### GUI Layer (`src/gui/`)
Static library `pointless_gui` (QML module) + `pointless` executable:
- `GuiController` - Main UI state (view type, navigation, filters, editing)
- `DataController` - Model-View sync, authentication, backend orchestration
- `TaskModel`, `TagModel`, `TaskFilterModel` - Qt models for QML
- `LocalSettings` - UI preferences via Qt settings

### QML Structure
- `Main.qml` - Main window with view switching
- `WeekView.qml` - Primary week-based task display
- `Task.qml` / `TaskView.qml` - Task rendering
- `EditTask.qml` - Task editing dialog
- `Style.qml` - Theme singleton (colors, spacing, fonts)

### Built-in Tags
`@Soon` (due within 7 days), `@Later` (no due date), `@Current` (today/tomorrow), `@Evening`

Use `BUILTIN_TAG_*` constants, `tagIsBuiltin()`, and `removeBuiltinTags()`. There is no "later" tag.

## Code Conventions

1. **File headers** - C++/header files must start with:
   ```cpp
   // SPDX-FileCopyrightText: 2025 Sergio Martins
   // SPDX-License-Identifier: MIT
   ```
   CMake files use `#` comments.

2. **No comments** - Code should be self-documenting.

3. **Logging** - Use `P_LOG_DEBUG`, `P_LOG_INFO`, `P_LOG_WARNING`, `P_LOG_ERROR` from `logger.h` with `{}` placeholders. Don't include function names (auto-included).

4. **Icons** - Use `FontAwesomeButton.qml` with FontAwesome icons. Avoid image files.

## Testing

- **Core tests**: `src/core/tests/` - gtest-based unit tests
- **GUI tests**: `src/gui/tests/` - Spix framework for Qt Quick testing
- GUI tests require `QT_QPA_PLATFORM=offscreen`

## Dependencies

Managed via vcpkg (`3rdparty/vcpkg/`):
- spdlog (logging), glaze (JSON), gtest (testing), cpr (HTTP), anyrpc (RPC)
- Qt6: Core, Gui, Quick, QuickControls2, Concurrent, Test

## Extra instructions

On macOS, read the relevant instructions from /Users/Shared/pub_data/etc/ai_prefs
