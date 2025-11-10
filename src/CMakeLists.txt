# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

add_subdirectory(core)

find_package(cpr CONFIG REQUIRED)

add_executable(pointless_cli cli/main.cpp)
target_link_libraries(pointless_cli PRIVATE pointless_core quill::quill
                                            cpr::cpr)
target_include_directories(pointless_cli
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
