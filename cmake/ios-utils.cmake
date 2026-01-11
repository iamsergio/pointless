# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

#[[
Sets iOS-specific Xcode attributes for a target related to manual code signing.

Parameters:
  target - The CMake target to configure

Sets the following properties:
  - XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER from APPLE_PROVISIONING_PROFILE_UUID env var
  - XCODE_ATTRIBUTE_DEVELOPMENT_TEAM from APPLE_TEAM_ID env var
#]]
function(ios_set_manual_signing_properties target)
  if(NOT DEFINED ENV{APPLE_PROVISIONING_PROFILE_UUID})
      message(FATAL_ERROR "APPLE_PROVISIONING_PROFILE_UUID environment variable is not set")
  endif()

  set_target_properties(${target} PROPERTIES
    XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER "$ENV{APPLE_PROVISIONING_PROFILE_UUID}"
    CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Distribution"
  )
endfunction()
