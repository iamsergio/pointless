# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

cmake_minimum_required(VERSION 3.21)
project(pointless_cpp LANGUAGES CXX)

enable_language(CXX)

set(POINTLESS_VERSION "1.0.0")
set(POINTLESS_APPLE_BUILD_NUMBER "1")

option(POINTLESS_DEVELOPER_MODE "Build with developer options" OFF)
option(POINTLESS_ENABLE_TESTS "Build tests" ${POINTLESS_DEVELOPER_MODE})

if(POINTLESS_ENABLE_TESTS)
  enable_testing()
  message(STATUS "Building with tests")
  find_package(GTest CONFIG REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(quill REQUIRED)

if(POINTLESS_ENABLE_ASAN)
  message(STATUS "Building with Address Sanitizer")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address,undefined")
endif()

if(POINTLESS_ENABLE_TSAN)
  message(STATUS "Building with Thread Sanitizer")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fno-omit-frame-pointer")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=thread")
endif()

if(IOS)

  # cmake-lint: disable=W0106
  if(NOT DEFINED ENV{APPLE_TEAM_ID})
    message(FATAL_ERROR "APPLE_TEAM_ID environment variable is not set")
  endif()

  set(BUNDLE_IDENTIFIER "com.iamsergio.pointless")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED YES)
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE
      "Automatic"
      CACHE STRING "Sign style")
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM
      "$ENV{APPLE_TEAM_ID}"
      CACHE STRING "iOS development team to use for signing")
  set(CMAKE_OSX_DEPLOYMENT_TARGET
      "17.0"
      CACHE STRING "Minimum iOS deployment target" FORCE)

  configure_file(${CMAKE_SOURCE_DIR}/src/ios/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist @ONLY)
endif()

add_subdirectory(src)
