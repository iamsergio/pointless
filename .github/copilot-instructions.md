---
applyTo: '**'
---

1. Cpp and Header files should start with a file comment block that has copyright information.
   Example:

// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

CMake files should also have the above, but with # instead of /* */ for comments.

2. Do not add comments

3. Logging
  - For logging use P_LOG_DEBUG, P_LOG_INFO, P_LOG_WARNING, P_LOG_ERROR macros from logger.h
  - Use {} for variable placeholders in log messages instead of %d, %s, etc.
  - When logging, don't include the function name as it's already included by the logging system.

4. Icons
   - Use FontAwesome icons via FontAwesomeButton.qml for buttons and icons in the GUI.
   - Avoid using image files unless asked
