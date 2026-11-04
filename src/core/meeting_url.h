// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>

namespace pointless::core {

/// Teams links win over Zoom, Google Meet and Webex, as Teams invites often embed links for those too.
/// Among links of the same kind, earlier texts win, so pass the most specific fields first.
[[nodiscard]] std::string findMeetingUrl(std::initializer_list<std::string_view> texts);

}
