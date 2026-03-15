# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

add_subdirectory(core)
add_subdirectory(gui)

if(NOT POINTLESS_YOCTO_BUILD)
  find_package(cpr CONFIG REQUIRED)
endif()
