# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

cmake_minimum_required(VERSION 3.21)
project(pointless_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


find_package(quill REQUIRED)

add_subdirectory(src)
