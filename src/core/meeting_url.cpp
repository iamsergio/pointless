// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "meeting_url.h"

#include <array>
#include <optional>
#include <vector>

namespace pointless::core {

namespace {

constexpr std::string_view urlTerminators = " \t\r\n<>\"'()[]|\\";

std::vector<std::string_view> extractUrls(std::string_view text)
{
    std::vector<std::string_view> urls;
    size_t pos = 0;
    while ((pos = text.find("http", pos)) != std::string_view::npos) {
        const auto rest = text.substr(pos);
        if (!rest.starts_with("https://") && !rest.starts_with("http://")) {
            pos += 4;
            continue;
        }

        const size_t end = text.find_first_of(urlTerminators, pos);
        urls.push_back(text.substr(pos, end == std::string_view::npos ? std::string_view::npos : end - pos));
        pos = end == std::string_view::npos ? text.size() : end;
    }

    return urls;
}

int hexValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

std::string percentDecode(std::string_view encoded)
{
    std::string decoded;
    decoded.reserve(encoded.size());
    for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.size()) {
            const int high = hexValue(encoded[i + 1]);
            const int low = hexValue(encoded[i + 2]);
            if (high >= 0 && low >= 0) {
                decoded.push_back(static_cast<char>((high << 4) | low));
                i += 2;
                continue;
            }
        }
        decoded.push_back(encoded[i]);
    }

    return decoded;
}

std::string_view hostOf(std::string_view url)
{
    const size_t schemeEnd = url.find("://");
    if (schemeEnd == std::string_view::npos)
        return {};

    const auto afterScheme = url.substr(schemeEnd + 3);
    return afterScheme.substr(0, afterScheme.find_first_of("/?#"));
}

bool hostIs(std::string_view host, std::string_view domain)
{
    return host == domain || (host.ends_with(domain) && host[host.size() - domain.size() - 1] == '.');
}

/// Outlook's "Safe Links" rewrite every link into https://<region>.safelinks.protection.outlook.com/?url=<encoded>&...
std::string unwrapSafeLink(std::string_view url)
{
    if (!hostIs(hostOf(url), "safelinks.protection.outlook.com"))
        return std::string(url);

    size_t start = url.find("?url=");
    if (start == std::string_view::npos)
        start = url.find("&url=");
    if (start == std::string_view::npos)
        return std::string(url);

    start += 5;
    const size_t end = url.find('&', start);
    return percentDecode(url.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start));
}

enum class MeetingKind {
    Teams,
    Zoom,
    GoogleMeet,
    Webex,
};

std::optional<MeetingKind> meetingKind(std::string_view url)
{
    const auto host = hostOf(url);
    const auto path = url.substr(url.find(host) + host.size());

    if ((host == "teams.microsoft.com" || host == "teams.live.com")
        && (path.starts_with("/l/meetup-join/") || path.starts_with("/meet/")))
        return MeetingKind::Teams;

    if (hostIs(host, "zoom.us") && (path.starts_with("/j/") || path.starts_with("/my/")))
        return MeetingKind::Zoom;

    if (host == "meet.google.com" && path.size() > 1)
        return MeetingKind::GoogleMeet;

    if (hostIs(host, "webex.com")
        && (path.starts_with("/meet/") || path.starts_with("/join/") || path.find("/j.php") != std::string_view::npos))
        return MeetingKind::Webex;

    return std::nullopt;
}

} // namespace

std::string findMeetingUrl(std::initializer_list<std::string_view> texts)
{
    constexpr std::array kindsByPriority = { MeetingKind::Teams, MeetingKind::Zoom, MeetingKind::GoogleMeet, MeetingKind::Webex };

    std::vector<std::pair<MeetingKind, std::string>> candidates;
    for (const auto text : texts) {
        for (const auto rawUrl : extractUrls(text)) {
            auto url = unwrapSafeLink(rawUrl);
            if (const auto kind = meetingKind(url))
                candidates.emplace_back(*kind, std::move(url));
        }
    }

    for (const auto kind : kindsByPriority) {
        for (const auto &[candidateKind, url] : candidates) {
            if (candidateKind == kind)
                return url;
        }
    }

    return {};
}

} // namespace pointless::core
