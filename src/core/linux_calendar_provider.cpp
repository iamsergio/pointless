// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "linux_calendar_provider.h"
#include "caldav_client.h"
#include "ical_parser.h"
#include "logger.h"
#include "utils.h"

#include <curl/curl.h>

namespace pointless::core {

namespace {

size_t icalWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    auto *response = static_cast<std::string *>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string fetchICalUrl(const std::string &url)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return {};

    std::string responseBody;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, icalWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        P_LOG_WARNING("Failed to fetch iCal URL {}: {}", url, curl_easy_strerror(res));
        return {};
    }

    return responseBody;
}

} // namespace

LinuxCalendarProvider::~LinuxCalendarProvider() = default;

LinuxCalendarProvider::LinuxCalendarProvider(std::vector<CalDavAccountConfig> accounts,
                                             std::vector<ICalUrlConfig> icalUrls)
{
    if (accounts.empty() && icalUrls.empty()) {
        auto url = pointless::getenv_or_empty("POINTLESS_CALDAV_URL");
        auto username = pointless::getenv_or_empty("POINTLESS_CALDAV_USERNAME");
        auto password = pointless::getenv_or_empty("POINTLESS_CALDAV_PASSWORD");

        if (url.empty()) {
            P_LOG_INFO("POINTLESS_CALDAV_URL not set, CalDAV integration disabled");
            return;
        }

        CalDavAccountConfig config;
        config.url = std::move(url);
        config.username = std::move(username);
        config.password = std::move(password);
        accounts.push_back(std::move(config));
    }

    for (auto &accountConfig : accounts) {
        CalDavConfig config;
        config.serverUrl = std::move(accountConfig.url);
        config.username = std::move(accountConfig.username);
        config.password = std::move(accountConfig.password);

        auto client = std::make_unique<CalDavClient>(std::move(config));
        auto homeSet = client->discoverCalendarHomeSet();
        if (!homeSet) {
            P_LOG_WARNING("Skipping account '{}': {}", accountConfig.name, homeSet.error());
            continue;
        }

        P_LOG_INFO("CalDAV calendar home set for '{}': {}", accountConfig.name, *homeSet);

        CalDavAccount account;
        account.name = std::move(accountConfig.name);
        account.client = std::move(client);
        account.homeSetUrl = std::move(*homeSet);
        m_accounts.push_back(std::move(account));
    }

    for (auto &cfg : icalUrls) {
        ICalSource source;
        source.url = std::move(cfg.url);
        source.name = cfg.name.empty() ? source.url : std::move(cfg.name);
        m_icalSources.push_back(std::move(source));
    }
}

bool LinuxCalendarProvider::isConfigured() const
{
    return !m_accounts.empty() || !m_icalSources.empty();
}

std::vector<Calendar> LinuxCalendarProvider::getCalendars() const
{
    std::vector<Calendar> allCalendars;
    for (const auto &account : m_accounts) {
        auto calendars = account.client->fetchCalendars(account.homeSetUrl);
        for (auto &cal : calendars) {
            cal.accountName = account.name;
        }
        allCalendars.insert(allCalendars.end(),
                            std::make_move_iterator(calendars.begin()),
                            std::make_move_iterator(calendars.end()));
    }

    for (const auto &source : m_icalSources) {
        Calendar cal;
        cal.id = source.url;
        cal.title = source.name;
        cal.writeable = false;
        cal.accountName = source.name;
        allCalendars.push_back(std::move(cal));
    }

    return allCalendars;
}

std::vector<CalendarEvent> LinuxCalendarProvider::getEvents(
    const DateRange &range,
    const std::vector<std::string> &calendarIds) const
{
    std::vector<CalendarEvent> allEvents;

    for (const auto &account : m_accounts) {
        auto calendars = account.client->fetchCalendars(account.homeSetUrl);

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

            std::string calendarUrl = CalDavClient::resolveUrl(account.homeSetUrl, cal.id);
            auto events = account.client->fetchEvents(calendarUrl, cal.id, cal.title, range);
            allEvents.insert(allEvents.end(),
                             std::make_move_iterator(events.begin()),
                             std::make_move_iterator(events.end()));
        }
    }

    for (const auto &source : m_icalSources) {
        bool requested = calendarIds.empty();
        if (!requested) {
            for (const auto &id : calendarIds) {
                if (id == source.url) {
                    requested = true;
                    break;
                }
            }
        }

        if (!requested)
            continue;

        std::string icalData = fetchICalUrl(source.url);
        if (icalData.empty())
            continue;

        auto parsed = parseICalEvents(icalData, range);
        for (auto &ev : parsed) {
            CalendarEvent ce;
            ce.eventId = ev.uid;
            ce.calendarId = source.url;
            ce.calendarName = source.name;
            ce.title = ev.summary;
            ce.startDate = ev.dtstart;
            ce.endDate = ev.dtend;
            ce.isAllDay = ev.isAllDay;
            allEvents.push_back(std::move(ce));
        }
    }

    return allEvents;
}

std::unique_ptr<CalendarProvider> createCalendarProvider(
    const std::string &caldavUrl,
    const std::string &caldavUsername,
    const std::string &caldavPassword)
{
    if (caldavUrl.empty() && caldavUsername.empty() && caldavPassword.empty())
        return std::make_unique<LinuxCalendarProvider>();

    CalDavAccountConfig config;
    config.url = caldavUrl;
    config.username = caldavUsername;
    config.password = caldavPassword;

    std::vector<CalDavAccountConfig> accounts;
    accounts.push_back(std::move(config));
    return std::make_unique<LinuxCalendarProvider>(std::move(accounts));
}

std::unique_ptr<CalendarProvider> createCalendarProvider(
    std::vector<CalDavAccountConfig> accounts)
{
    return std::make_unique<LinuxCalendarProvider>(std::move(accounts));
}

std::unique_ptr<CalendarProvider> createCalendarProvider(
    std::vector<CalDavAccountConfig> accounts,
    std::vector<ICalUrlConfig> icalUrls)
{
    return std::make_unique<LinuxCalendarProvider>(std::move(accounts), std::move(icalUrls));
}

} // namespace pointless::core
