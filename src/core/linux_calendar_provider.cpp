// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "linux_calendar_provider.h"
#include "caldav_client.h"
#include "logger.h"
#include "utils.h"

namespace pointless::core {

LinuxCalendarProvider::~LinuxCalendarProvider() = default;

LinuxCalendarProvider::LinuxCalendarProvider()
{
    auto url = pointless::getenv_or_empty("POINTLESS_CALDAV_URL");
    auto username = pointless::getenv_or_empty("POINTLESS_CALDAV_USERNAME");
    auto password = pointless::getenv_or_empty("POINTLESS_CALDAV_PASSWORD");

    if (url.empty()) {
        P_LOG_INFO("POINTLESS_CALDAV_URL not set, CalDAV integration disabled");
        return;
    }

    CalDavConfig config;
    config.serverUrl = std::move(url);
    config.username = std::move(username);
    config.password = std::move(password);

    m_client = std::make_unique<CalDavClient>(std::move(config));
    m_homeSetUrl = m_client->discoverCalendarHomeSet();
    P_LOG_INFO("CalDAV calendar home set: {}", m_homeSetUrl);
}

std::vector<Calendar> LinuxCalendarProvider::getCalendars() const
{
    if (!m_client)
        return {};

    return m_client->fetchCalendars(m_homeSetUrl);
}

std::vector<CalendarEvent> LinuxCalendarProvider::getEvents(
    const DateRange &range,
    const std::vector<std::string> &calendarIds) const
{
    if (!m_client)
        return {};

    auto calendars = m_client->fetchCalendars(m_homeSetUrl);

    std::vector<CalendarEvent> allEvents;
    for (const auto &cal : calendars) {
        bool requested = calendarIds.empty();
        if (!requested) {
            for (const auto &id : calendarIds) {
                if (id == cal.id) {
                    requested = true;
                    break;
                }
            }
        }

        if (!requested)
            continue;

        std::string calendarUrl;
        if (cal.id.starts_with("http://") || cal.id.starts_with("https://")) {
            calendarUrl = cal.id;
        } else {
            auto schemeEnd = m_homeSetUrl.find("://");
            if (schemeEnd != std::string::npos) {
                auto hostEnd = m_homeSetUrl.find('/', schemeEnd + 3);
                calendarUrl = m_homeSetUrl.substr(0, hostEnd) + cal.id;
            } else {
                calendarUrl = cal.id;
            }
        }

        auto events = m_client->fetchEvents(calendarUrl, cal.id, cal.title, range);
        allEvents.insert(allEvents.end(),
                         std::make_move_iterator(events.begin()),
                         std::make_move_iterator(events.end()));
    }

    return allEvents;
}

std::unique_ptr<CalendarProvider> createCalendarProvider()
{
    return std::make_unique<LinuxCalendarProvider>();
}

} // namespace pointless::core
