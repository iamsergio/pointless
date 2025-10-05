# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT


add_subdirectory(core)

add_executable(pointless_cli cli/main.cpp)
target_link_libraries(pointless_cli PRIVATE pointless_core)
target_include_directories(pointless_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
