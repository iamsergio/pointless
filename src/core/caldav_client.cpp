// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "caldav_client.h"
#include "ical_parser.h"
#include "logger.h"

#include <cpr/cpr.h>
#include <curl/curl.h>
#include <pugixml.hpp>

#include <chrono>
#include <format>

namespace pointless::core {

namespace {

void ensureTrailingSlash(std::string &url)
{
    if (!url.empty() && url.back() != '/')
        url += '/';
}

std::string localName(const std::string &name)
{
    auto pos = name.find(':');
    if (pos != std::string::npos)
        return name.substr(pos + 1);
    return name;
}

pugi::xml_node findChildByLocalName(pugi::xml_node parent, const std::string &name)
{
    for (auto child : parent.children()) {
        if (localName(child.name()) == name)
            return child;
    }
    return {};
}

pugi::xml_node findDescendantByLocalName(pugi::xml_node parent, const std::string &name)
{
    std::vector<pugi::xml_node> stack = { parent };
    while (!stack.empty()) {
        auto node = stack.back();
        stack.pop_back();
        if (localName(node.name()) == name)
            return node;
        for (auto child : node.children())
            stack.push_back(child);
    }
    return {};
}

std::string formatTimeRange(const DateRange &range)
{
    auto formatTime = [](std::chrono::system_clock::time_point tp) -> std::string {
        time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm = {};
        if (!gmtime_r(&t, &tm))
            return {};
        return std::format("{:04}{:02}{:02}T{:02}{:02}{:02}Z",
                           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                           tm.tm_hour, tm.tm_min, tm.tm_sec);
    };

    return std::format(R"(start="{}" end="{}")", formatTime(range.start), formatTime(range.end));
}

std::string normalizeColor(const std::string &color)
{
    if (color.size() == 9 && color[0] == '#')
        return color.substr(0, 7);
    return color;
}

} // namespace

CalDavClient::CalDavClient(CalDavConfig config)
    : m_config(std::move(config))
{
    ensureTrailingSlash(m_config.serverUrl);
}

std::string CalDavClient::propfind(const std::string &url, const std::string &body, int depth) const
{
    cpr::Session session;
    session.SetUrl(cpr::Url { url });
    session.SetHeader(cpr::Header {
        { "Content-Type", "application/xml; charset=utf-8" },
        { "Depth", std::to_string(depth) } });
    session.SetBody(cpr::Body { body });
    session.SetAuth(cpr::Authentication { m_config.username, m_config.password, cpr::AuthMode::BASIC });

    curl_easy_setopt(session.GetCurlHolder()->handle, CURLOPT_CUSTOMREQUEST, "PROPFIND");
    auto response = session.Post();

    if (response.status_code < 200 || response.status_code >= 400) {
        P_LOG_WARNING("PROPFIND {} failed with status {}", url, response.status_code);
        return {};
    }

    return response.text;
}

std::string CalDavClient::report(const std::string &url, const std::string &body) const
{
    cpr::Session session;
    session.SetUrl(cpr::Url { url });
    session.SetHeader(cpr::Header {
        { "Content-Type", "application/xml; charset=utf-8" },
        { "Depth", "1" } });
    session.SetBody(cpr::Body { body });
    session.SetAuth(cpr::Authentication { m_config.username, m_config.password, cpr::AuthMode::BASIC });

    curl_easy_setopt(session.GetCurlHolder()->handle, CURLOPT_CUSTOMREQUEST, "REPORT");
    auto response = session.Post();

    if (response.status_code < 200 || response.status_code >= 400) {
        P_LOG_WARNING("REPORT {} failed with status {}", url, response.status_code);
        return {};
    }

    return response.text;
}

std::string CalDavClient::discoverCalendarHomeSet() const
{
    std::string body = R"(<?xml version="1.0" encoding="utf-8" ?>
<d:propfind xmlns:d="DAV:" xmlns:c="urn:ietf:params:xml:ns:caldav">
  <d:prop>
    <c:calendar-home-set />
  </d:prop>
</d:propfind>)";

    auto xml = propfind(m_config.serverUrl, body, 0);
    if (xml.empty())
        return m_config.serverUrl;

    pugi::xml_document doc;
    if (!doc.load_string(xml.c_str())) {
        P_LOG_WARNING("Failed to parse calendar-home-set response");
        return m_config.serverUrl;
    }

    auto homeSetNode = findDescendantByLocalName(doc.root(), "calendar-home-set");
    if (!homeSetNode) {
        P_LOG_DEBUG("No calendar-home-set found, using server URL directly");
        return m_config.serverUrl;
    }

    auto hrefNode = findChildByLocalName(homeSetNode, "href");
    if (!hrefNode) {
        P_LOG_DEBUG("No href in calendar-home-set, using server URL directly");
        return m_config.serverUrl;
    }

    std::string href = hrefNode.child_value();
    if (href.empty())
        return m_config.serverUrl;

    if (href.starts_with("http://") || href.starts_with("https://"))
        return href;

    auto schemeEnd = m_config.serverUrl.find("://");
    if (schemeEnd == std::string::npos)
        return href;
    auto hostEnd = m_config.serverUrl.find('/', schemeEnd + 3);
    std::string host = m_config.serverUrl.substr(0, hostEnd);
    return host + href;
}

std::vector<Calendar> CalDavClient::fetchCalendars(const std::string &homeSetUrl) const
{
    std::string body = R"(<?xml version="1.0" encoding="utf-8" ?>
<d:propfind xmlns:d="DAV:" xmlns:c="urn:ietf:params:xml:ns:caldav" xmlns:cs="http://apple.com/ns/ical/">
  <d:prop>
    <d:displayname />
    <d:resourcetype />
    <c:supported-calendar-component-set />
    <cs:calendar-color />
  </d:prop>
</d:propfind>)";

    auto xml = propfind(homeSetUrl, body, 1);
    if (xml.empty())
        return {};

    pugi::xml_document doc;
    if (!doc.load_string(xml.c_str())) {
        P_LOG_WARNING("Failed to parse calendars response");
        return {};
    }

    std::vector<Calendar> calendars;

    auto multistatus = findDescendantByLocalName(doc.root(), "multistatus");
    if (!multistatus)
        return {};

    for (auto response : multistatus.children()) {
        if (localName(response.name()) != "response")
            continue;

        auto propstat = findDescendantByLocalName(response, "propstat");
        if (!propstat)
            continue;

        auto prop = findChildByLocalName(propstat, "prop");
        if (!prop)
            continue;

        auto resourcetype = findChildByLocalName(prop, "resourcetype");
        if (!resourcetype)
            continue;

        bool isCalendar = false;
        for (auto child : resourcetype.children()) {
            if (localName(child.name()) == "calendar") {
                isCalendar = true;
                break;
            }
        }
        if (!isCalendar)
            continue;

        auto supComp = findChildByLocalName(prop, "supported-calendar-component-set");
        if (supComp) {
            bool supportsVEvent = false;
            for (auto comp : supComp.children()) {
                if (localName(comp.name()) == "comp") {
                    std::string compName = comp.attribute("name").as_string();
                    if (compName == "VEVENT") {
                        supportsVEvent = true;
                        break;
                    }
                }
            }
            if (!supportsVEvent)
                continue;
        }

        auto hrefNode = findChildByLocalName(response, "href");
        if (!hrefNode)
            continue;

        Calendar cal;
        cal.id = hrefNode.child_value();

        auto displayname = findChildByLocalName(prop, "displayname");
        if (displayname)
            cal.title = displayname.child_value();

        auto colorNode = findChildByLocalName(prop, "calendar-color");
        if (colorNode)
            cal.color = normalizeColor(colorNode.child_value());

        calendars.push_back(std::move(cal));
    }

    return calendars;
}

std::vector<CalendarEvent> CalDavClient::fetchEvents(
    const std::string &calendarUrl,
    const std::string &calendarId,
    const std::string &calendarName,
    const DateRange &range) const
{
    std::string timeRange = formatTimeRange(range);
    std::string body = std::format(R"(<?xml version="1.0" encoding="utf-8" ?>
<c:calendar-query xmlns:d="DAV:" xmlns:c="urn:ietf:params:xml:ns:caldav">
  <d:prop>
    <d:getetag />
    <c:calendar-data />
  </d:prop>
  <c:filter>
    <c:comp-filter name="VCALENDAR">
      <c:comp-filter name="VEVENT">
        <c:time-range {} />
      </c:comp-filter>
    </c:comp-filter>
  </c:filter>
</c:calendar-query>)",
                                   timeRange);

    auto xml = report(calendarUrl, body);
    if (xml.empty())
        return {};

    pugi::xml_document doc;
    if (!doc.load_string(xml.c_str())) {
        P_LOG_WARNING("Failed to parse events response");
        return {};
    }

    std::vector<CalendarEvent> events;

    auto multistatus = findDescendantByLocalName(doc.root(), "multistatus");
    if (!multistatus)
        return {};

    for (auto response : multistatus.children()) {
        if (localName(response.name()) != "response")
            continue;

        auto calData = findDescendantByLocalName(response, "calendar-data");
        if (!calData)
            continue;

        std::string icalData = calData.child_value();
        if (icalData.empty())
            continue;

        auto parsed = parseICalEvents(icalData);
        for (auto &ev : parsed) {
            CalendarEvent ce;
            ce.eventId = ev.uid;
            ce.calendarId = calendarId;
            ce.calendarName = calendarName;
            ce.title = ev.summary;
            ce.startDate = ev.dtstart;
            ce.endDate = ev.dtend;
            ce.isAllDay = ev.isAllDay;
            events.push_back(std::move(ce));
        }
    }

    return events;
}

} // namespace pointless::core
