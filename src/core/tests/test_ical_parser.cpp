// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "ical_parser.h"

#include <gtest/gtest.h>

#include <chrono>

using namespace pointless::core;

namespace {

void expectUTC(const std::chrono::system_clock::time_point &tp,
               int year, int month, int day, int hour, int minute, int second)
{
    auto t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = {};
    gmtime_r(&t, &tm);
    EXPECT_EQ(tm.tm_year + 1900, year);
    EXPECT_EQ(tm.tm_mon + 1, month);
    EXPECT_EQ(tm.tm_mday, day);
    EXPECT_EQ(tm.tm_hour, hour);
    EXPECT_EQ(tm.tm_min, minute);
    EXPECT_EQ(tm.tm_sec, second);
}

} // namespace

TEST(ICalParserTest, SimpleEvent)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:abc-123
SUMMARY:Team Meeting
DTSTART:20250115T100000Z
DTEND:20250115T110000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "abc-123");
    EXPECT_EQ(events[0].summary, "Team Meeting");
    EXPECT_FALSE(events[0].isAllDay);

    expectUTC(events[0].dtstart, 2025, 1, 15, 10, 0, 0);
    expectUTC(events[0].dtend, 2025, 1, 15, 11, 0, 0);
}

TEST(ICalParserTest, AllDayEvent)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:allday-1
SUMMARY:Holiday
DTSTART;VALUE=DATE:20250320
DTEND;VALUE=DATE:20250321
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "allday-1");
    EXPECT_EQ(events[0].summary, "Holiday");
    EXPECT_TRUE(events[0].isAllDay);
}

TEST(ICalParserTest, MultipleEvents)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:ev-1
SUMMARY:First
DTSTART:20250201T090000Z
DTEND:20250201T100000Z
END:VEVENT
BEGIN:VEVENT
UID:ev-2
SUMMARY:Second
DTSTART:20250201T140000Z
DTEND:20250201T150000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].uid, "ev-1");
    EXPECT_EQ(events[0].summary, "First");
    EXPECT_EQ(events[1].uid, "ev-2");
    EXPECT_EQ(events[1].summary, "Second");
}

TEST(ICalParserTest, LineUnfolding)
{
    const std::string ical = "BEGIN:VCALENDAR\r\n"
                             "VERSION:2.0\r\n"
                             "BEGIN:VEVENT\r\n"
                             "UID:fold-1\r\n"
                             "SUMMARY:A very long summary that has been \r\n"
                             " folded across multiple lines\r\n"
                             "DTSTART:20250301T120000Z\r\n"
                             "DTEND:20250301T130000Z\r\n"
                             "END:VEVENT\r\n"
                             "END:VCALENDAR\r\n";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "fold-1");
    EXPECT_EQ(events[0].summary, "A very long summary that has been folded across multiple lines");
}

TEST(ICalParserTest, EmptyInput)
{
    auto events = parseICalEvents("");
    EXPECT_TRUE(events.empty());
}

TEST(ICalParserTest, InvalidInput)
{
    auto events = parseICalEvents("not ical data");
    EXPECT_TRUE(events.empty());
}

TEST(ICalParserTest, AllDayMultiDay)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:multiday-1
SUMMARY:Conference
DTSTART;VALUE=DATE:20250610
DTEND;VALUE=DATE:20250613
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "multiday-1");
    EXPECT_EQ(events[0].summary, "Conference");
    EXPECT_TRUE(events[0].isAllDay);

    expectUTC(events[0].dtstart, 2025, 6, 10, 0, 0, 0);
    expectUTC(events[0].dtend, 2025, 6, 13, 0, 0, 0);
}

TEST(ICalParserTest, MissingUid)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
SUMMARY:No UID Event
DTSTART:20250501T080000Z
DTEND:20250501T090000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_TRUE(events[0].uid.empty());
    EXPECT_EQ(events[0].summary, "No UID Event");
}

TEST(ICalParserTest, MissingSummary)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:no-summary-1
DTSTART:20250501T080000Z
DTEND:20250501T090000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "no-summary-1");
    EXPECT_TRUE(events[0].summary.empty());
}

TEST(ICalParserTest, MissingDtend)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:no-end-1
SUMMARY:Start Only
DTSTART:20250701T150000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "no-end-1");
    expectUTC(events[0].dtstart, 2025, 7, 1, 15, 0, 0);
}

TEST(ICalParserTest, MidnightBoundary)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:midnight-1
SUMMARY:Late Night
DTSTART:20250228T230000Z
DTEND:20250301T010000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    expectUTC(events[0].dtstart, 2025, 2, 28, 23, 0, 0);
    expectUTC(events[0].dtend, 2025, 3, 1, 1, 0, 0);
}

TEST(ICalParserTest, NonUtcTimestamp)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:nonutc-1
SUMMARY:Floating Time
DTSTART:20250415T143000
DTEND:20250415T153000
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "nonutc-1");
    EXPECT_FALSE(events[0].isAllDay);
}

TEST(ICalParserTest, ExtraProperties)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
PRODID:-//Test//Test//EN
CALSCALE:GREGORIAN
BEGIN:VEVENT
UID:extra-1
SUMMARY:With Extras
DESCRIPTION:A detailed description of the event
LOCATION:Conference Room A
DTSTART:20250520T100000Z
DTEND:20250520T113000Z
STATUS:CONFIRMED
ORGANIZER;CN=John:mailto:john@example.com
ATTENDEE;CN=Jane:mailto:jane@example.com
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "extra-1");
    EXPECT_EQ(events[0].summary, "With Extras");
    expectUTC(events[0].dtstart, 2025, 5, 20, 10, 0, 0);
    expectUTC(events[0].dtend, 2025, 5, 20, 11, 30, 0);
}

TEST(ICalParserTest, VcalendarWithNoVevents)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VTODO
UID:todo-1
SUMMARY:A Task
END:VTODO
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    EXPECT_TRUE(events.empty());
}

TEST(ICalParserTest, CrlfLineEndings)
{
    const std::string ical = "BEGIN:VCALENDAR\r\n"
                             "VERSION:2.0\r\n"
                             "BEGIN:VEVENT\r\n"
                             "UID:crlf-1\r\n"
                             "SUMMARY:CRLF Event\r\n"
                             "DTSTART:20250801T060000Z\r\n"
                             "DTEND:20250801T070000Z\r\n"
                             "END:VEVENT\r\n"
                             "END:VCALENDAR\r\n";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].uid, "crlf-1");
    EXPECT_EQ(events[0].summary, "CRLF Event");
    expectUTC(events[0].dtstart, 2025, 8, 1, 6, 0, 0);
}

TEST(ICalParserTest, SpecialCharactersInSummary)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:special-1
SUMMARY:Meeting: Q1 Review & Planning (2025)
DTSTART:20250310T140000Z
DTEND:20250310T160000Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].summary, "Meeting: Q1 Review & Planning (2025)");
}

TEST(ICalParserTest, MixedAllDayAndTimedEvents)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:mixed-1
SUMMARY:All Day
DTSTART;VALUE=DATE:20250901
DTEND;VALUE=DATE:20250902
END:VEVENT
BEGIN:VEVENT
UID:mixed-2
SUMMARY:Timed
DTSTART:20250901T100000Z
DTEND:20250901T110000Z
END:VEVENT
BEGIN:VEVENT
UID:mixed-3
SUMMARY:Another All Day
DTSTART;VALUE=DATE:20250902
DTEND;VALUE=DATE:20250903
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 3);

    EXPECT_EQ(events[0].uid, "mixed-1");
    EXPECT_TRUE(events[0].isAllDay);

    EXPECT_EQ(events[1].uid, "mixed-2");
    EXPECT_FALSE(events[1].isAllDay);
    expectUTC(events[1].dtstart, 2025, 9, 1, 10, 0, 0);

    EXPECT_EQ(events[2].uid, "mixed-3");
    EXPECT_TRUE(events[2].isAllDay);
}

TEST(ICalParserTest, NonSecondBoundaryTimes)
{
    const std::string ical = R"(BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:odd-time-1
SUMMARY:Odd Times
DTSTART:20251225T073015Z
DTEND:20251225T094545Z
END:VEVENT
END:VCALENDAR)";

    auto events = parseICalEvents(ical);
    ASSERT_EQ(events.size(), 1);
    expectUTC(events[0].dtstart, 2025, 12, 25, 7, 30, 15);
    expectUTC(events[0].dtend, 2025, 12, 25, 9, 45, 45);
}
