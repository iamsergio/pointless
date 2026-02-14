// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "ical_parser.h"

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include <functional>

using namespace pointless::core;

TEST(CalDavXmlTest, CalendarHomeSetParsing)
{
    const std::string xml = R"(<?xml version="1.0" encoding="utf-8" ?>
<d:multistatus xmlns:d="DAV:" xmlns:cal="urn:ietf:params:xml:ns:caldav">
  <d:response>
    <d:href>/principals/user/</d:href>
    <d:propstat>
      <d:prop>
        <cal:calendar-home-set>
          <d:href>/calendars/user/</d:href>
        </cal:calendar-home-set>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
</d:multistatus>)";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml.c_str()));

    auto findByLocal = [](pugi::xml_node parent, const std::string &name) -> pugi::xml_node {
        auto localName = [](const std::string &n) {
            auto pos = n.find(':');
            return pos != std::string::npos ? n.substr(pos + 1) : n;
        };

        if (localName(parent.name()) == name)
            return parent;
        for (auto child : parent.children()) {
            pugi::xml_node found;
            std::function<pugi::xml_node(pugi::xml_node, const std::string &)> search;
            search = [&](pugi::xml_node node, const std::string &n) -> pugi::xml_node {
                if (localName(node.name()) == n)
                    return node;
                for (auto c : node.children()) {
                    auto r = search(c, n);
                    if (r) return r;
                }
                return {};
            };
            found = search(child, name);
            if (found) return found;
        }
        return {};
    };

    auto homeSet = findByLocal(doc, "calendar-home-set");
    ASSERT_TRUE(homeSet);

    auto href = findByLocal(homeSet, "href");
    ASSERT_TRUE(href);
    EXPECT_STREQ(href.child_value(), "/calendars/user/");
}

TEST(CalDavXmlTest, CalendarListParsing)
{
    const std::string xml = R"(<?xml version="1.0" encoding="utf-8" ?>
<d:multistatus xmlns:d="DAV:" xmlns:cal="urn:ietf:params:xml:ns:caldav" xmlns:cs="http://apple.com/ns/ical/">
  <d:response>
    <d:href>/calendars/user/personal/</d:href>
    <d:propstat>
      <d:prop>
        <d:displayname>Personal</d:displayname>
        <d:resourcetype>
          <d:collection />
          <cal:calendar />
        </d:resourcetype>
        <cal:supported-calendar-component-set>
          <cal:comp name="VEVENT" />
        </cal:supported-calendar-component-set>
        <cs:calendar-color>#FF5733FF</cs:calendar-color>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
  <d:response>
    <d:href>/calendars/user/tasks/</d:href>
    <d:propstat>
      <d:prop>
        <d:displayname>Tasks</d:displayname>
        <d:resourcetype>
          <d:collection />
          <cal:calendar />
        </d:resourcetype>
        <cal:supported-calendar-component-set>
          <cal:comp name="VTODO" />
        </cal:supported-calendar-component-set>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
</d:multistatus>)";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml.c_str()));

    auto localName = [](const std::string &name) {
        auto pos = name.find(':');
        return pos != std::string::npos ? name.substr(pos + 1) : name;
    };

    std::function<pugi::xml_node(pugi::xml_node, const std::string &)> findByLocal;
    findByLocal = [&](pugi::xml_node parent, const std::string &name) -> pugi::xml_node {
        if (localName(parent.name()) == name)
            return parent;
        for (auto child : parent.children()) {
            auto found = findByLocal(child, name);
            if (found) return found;
        }
        return {};
    };

    auto findChildByLocal = [&](pugi::xml_node parent, const std::string &name) -> pugi::xml_node {
        for (auto child : parent.children()) {
            if (localName(child.name()) == name)
                return child;
        }
        return {};
    };

    auto multistatus = findByLocal(doc, "multistatus");
    ASSERT_TRUE(multistatus);

    int calendarCount = 0;
    int veventCalendarCount = 0;

    for (auto response : multistatus.children()) {
        if (localName(response.name()) != "response")
            continue;

        auto propstat = findByLocal(response, "propstat");
        ASSERT_TRUE(propstat);

        auto prop = findChildByLocal(propstat, "prop");
        ASSERT_TRUE(prop);

        auto resourcetype = findChildByLocal(prop, "resourcetype");
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
        calendarCount++;

        auto supComp = findChildByLocal(prop, "supported-calendar-component-set");
        if (supComp) {
            for (auto comp : supComp.children()) {
                if (localName(comp.name()) == "comp" &&
                    std::string(comp.attribute("name").as_string()) == "VEVENT") {
                    veventCalendarCount++;
                    break;
                }
            }
        }
    }

    EXPECT_EQ(calendarCount, 2);
    EXPECT_EQ(veventCalendarCount, 1);
}

TEST(CalDavXmlTest, CalendarDataExtraction)
{
    const std::string xml = R"(<?xml version="1.0" encoding="utf-8" ?>
<d:multistatus xmlns:d="DAV:" xmlns:cal="urn:ietf:params:xml:ns:caldav">
  <d:response>
    <d:href>/calendars/user/personal/event1.ics</d:href>
    <d:propstat>
      <d:prop>
        <d:getetag>"etag-123"</d:getetag>
        <cal:calendar-data>BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:xml-ev-1
SUMMARY:Extracted Event
DTSTART:20250401T080000Z
DTEND:20250401T090000Z
END:VEVENT
END:VCALENDAR</cal:calendar-data>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
</d:multistatus>)";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml.c_str()));

    std::function<pugi::xml_node(pugi::xml_node, const std::string &)> findByLocal;
    findByLocal = [&](pugi::xml_node parent, const std::string &name) -> pugi::xml_node {
        auto localName = [](const std::string &n) {
            auto pos = n.find(':');
            return pos != std::string::npos ? n.substr(pos + 1) : n;
        };
        if (localName(parent.name()) == name)
            return parent;
        for (auto child : parent.children()) {
            auto found = findByLocal(child, name);
            if (found) return found;
        }
        return {};
    };

    auto calData = findByLocal(doc, "calendar-data");
    ASSERT_TRUE(calData);

    std::string icalData = calData.child_value();
    ASSERT_FALSE(icalData.empty());

    auto events = parseICalEvents(icalData);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "xml-ev-1");
    EXPECT_EQ(events[0].summary, "Extracted Event");
}
