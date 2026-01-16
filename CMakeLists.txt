# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

include(cmake/ios-utils.cmake)

if(POINTLESS_IOS_BUILD)
  if(NOT DEFINED ENV{QT_INSTALL})
    message(FATAL_ERROR "QT_INSTALL environment variable is not set")
  endif()

  set(QT_HOST_PATH_CANDIDATE "$ENV{QT_INSTALL}/../macos/")
  if(NOT EXISTS "${QT_HOST_PATH_CANDIDATE}")
    message(FATAL_ERROR "Qt host path does not exist: ${QT_HOST_PATH_CANDIDATE}")
  endif()

  set(IOS
      TRUE
      CACHE BOOL "Build for iOS")

  set(CMAKE_SYSTEM_NAME
      "iOS"
      CACHE STRING "System name" FORCE)

  set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
      "$ENV{QT_INSTALL}/lib/cmake/Qt6/qt.toolchain.cmake"
      CACHE FILEPATH "Qt toolchain file")

  set(QT_HOST_PATH
      "${QT_HOST_PATH_CANDIDATE}"
      CACHE PATH "Qt host path")
  set(CMAKE_OSX_DEPLOYMENT_TARGET
      "17.0"
      CACHE STRING "Minimum iOS deployment target" FORCE)
  set(CMAKE_OSX_ARCHITECTURES
      "arm64"
      CACHE STRING "iOS architecture")
  set(CMAKE_OSX_SYSROOT
      "iphoneos"
      CACHE STRING "iOS SDK")
  set(CMAKE_XCODE_GENERATE_SCHEME
      "ON"
      CACHE BOOL "Generate Xcode scheme")
  set(VCPKG_TARGET_TRIPLET
      "arm64-ios"
      CACHE STRING "vcpkg triplet")
  # Sets for all targets
  set(CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.iamsergio.pointless")

  set(
      # Sets the app to iPhone only (Value "1"). No iPad support yet.
      CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1"
      CACHE STRING "iOS targeted device family" FORCE)
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

cmake_minimum_required(VERSION 3.21)
project(pointless_cpp LANGUAGES CXX)

enable_language(CXX)

if(CMAKE_CXX_CLANG_TIDY)
  # suppress warnings from build dir, such as vcpkg stuff
  file(COPY "${CMAKE_SOURCE_DIR}/src/core/tests/.clang-tidy" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

include(cmake/CompilerFlags.cmake)

set(POINTLESS_VERSION "1.0.0")
string(TIMESTAMP POINTLESS_APPLE_BUILD_NUMBER "%y%m%d%H%M")
option(POINTLESS_DEVELOPER_MODE "Build with developer options" OFF)
option(POINTLESS_ENABLE_TESTS "Build tests" ${POINTLESS_DEVELOPER_MODE})
option(POINTLESS_VERIFY_SSL "Verify SSL certificates in HTTPS requests" ON)

if(POINTLESS_DEVELOPER_MODE)
  message(STATUS "Building in developer mode")
  add_definitions(-DPOINTLESS_DEVELOPER_MODE)
endif()

if(NOT POINTLESS_VERIFY_SSL)
  message(STATUS "SSL verification disabled")
  add_definitions(-DPOINTLESS_VERIFY_SSL=0)
else()
  add_definitions(-DPOINTLESS_VERIFY_SSL=1)
endif()

if(POINTLESS_ENABLE_TESTS)
  enable_testing()
  message(STATUS "Building with tests")
  find_package(GTest CONFIG REQUIRED)
  find_package(Qt6Test REQUIRED)

  add_definitions(-DPOINTLESS_ENABLE_TESTS)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(spdlog CONFIG REQUIRED)

set(QT_QML_GENERATE_QMLLS_INI TRUE)

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

  if(POINTLESS_DEVELOPER_MODE)
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE
        "Automatic"
        CACHE STRING "Sign style")
  else()
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE
        "manual"
        CACHE STRING "Sign style")
  endif()

  set(BUNDLE_IDENTIFIER "com.iamsergio.pointless")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED YES)
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM
      "$ENV{APPLE_TEAM_ID}"
      CACHE STRING "iOS development team to use for signing")

  configure_file(${CMAKE_SOURCE_DIR}/src/ios/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist @ONLY)
  configure_file(${CMAKE_SOURCE_DIR}/src/ios/ExportOptions-TestFlight.plist.in ${CMAKE_BINARY_DIR}/ExportOptions-TestFlight.plist @ONLY)
endif()

if(POINTLESS_ENABLE_TESTS)
  add_subdirectory(3rdparty/spix)
  target_compile_options(SpixCore PRIVATE -Wno-deprecated-declarations)
  target_compile_options(SpixQtQuick PRIVATE -Wno-deprecated-declarations)
endif()

add_subdirectory(src)

pointless_cpp_set_compiler_flags(pointless_core ${POINTLESS_DEVELOPER_MODE})
pointless_cpp_set_compiler_flags(pointless_gui ${POINTLESS_DEVELOPER_MODE})
pointless_cpp_set_compiler_flags(pointless ${POINTLESS_DEVELOPER_MODE})
