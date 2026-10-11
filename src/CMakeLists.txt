# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

add_subdirectory(core)
add_subdirectory(gui)

if(POINTLESS_SLINT_OVER_QTCORE)
  add_subdirectory(slint/bridge)
endif()

if(NOT POINTLESS_YOCTO_BUILD)
  find_package(cpr CONFIG REQUIRED)
endif()
