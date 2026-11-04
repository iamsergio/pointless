// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "meeting_url.h"

#include <gtest/gtest.h>

using namespace pointless::core;

namespace {
constexpr std::string_view teamsJoinUrl = "https://teams.microsoft.com/l/meetup-join/19%3ameeting_abc%40thread.v2/0?context=%7b%22Tid%22%3a%2242%22%7d";
}

TEST(MeetingUrlTest, NoTexts)
{
    EXPECT_EQ(findMeetingUrl({}), "");
    EXPECT_EQ(findMeetingUrl({ "", "" }), "");
}

TEST(MeetingUrlTest, NoMeetingLinks)
{
    EXPECT_EQ(findMeetingUrl({ "Microsoft Teams Meeting", "See https://example.com/agenda and http://intranet/wiki" }), "");
}

TEST(MeetingUrlTest, TeamsInviteSkipsHelpLink)
{
    const std::string description = std::string("Microsoft Teams Need help?<https://aka.ms/JoinTeamsMeeting?omkt=en-US>\n"
                                                "Join the meeting now<")
        + std::string(teamsJoinUrl) + ">\nMeeting ID: 123";

    EXPECT_EQ(findMeetingUrl({ description }), teamsJoinUrl);
}

TEST(MeetingUrlTest, TeamsShortLink)
{
    EXPECT_EQ(findMeetingUrl({ "Join: https://teams.microsoft.com/meet/2505243695132?p=Abc123 now" }),
              "https://teams.microsoft.com/meet/2505243695132?p=Abc123");
}

TEST(MeetingUrlTest, TeamsNonJoinLinksAreIgnored)
{
    EXPECT_EQ(findMeetingUrl({ "https://teams.microsoft.com/meetingOptions/?organizerId=1 "
                               "https://teams.microsoft.com/l/channel/19%3aabc/General" }),
              "");
}

TEST(MeetingUrlTest, SafeLinksAreUnwrapped)
{
    const std::string description = "Join<https://eur02.safelinks.protection.outlook.com/?url=https%3A%2F%2Fteams.microsoft.com"
                                    "%2Fl%2Fmeetup-join%2F19%253ameeting_abc%2540thread.v2%2F0&data=05%7C01&reserved=0>";

    EXPECT_EQ(findMeetingUrl({ description }), "https://teams.microsoft.com/l/meetup-join/19%3ameeting_abc%40thread.v2/0");
}

TEST(MeetingUrlTest, TeamsWinsOverWebex)
{
    const std::string description = "Webex: https://acme.webex.com/acme/j.php?MTID=m123\n"
                                    "Teams: "
        + std::string(teamsJoinUrl);

    EXPECT_EQ(findMeetingUrl({ description }), teamsJoinUrl);
}

TEST(MeetingUrlTest, EarlierTextWinsForSameKind)
{
    EXPECT_EQ(findMeetingUrl({ "https://teams.microsoft.com/meet/111?p=a", "https://teams.microsoft.com/meet/222?p=b" }),
              "https://teams.microsoft.com/meet/111?p=a");
}

TEST(MeetingUrlTest, ZoomInLocation)
{
    EXPECT_EQ(findMeetingUrl({ "message://%3Cabc%40mail%3E", "https://zoom.us/j/81132435236?pwd=xyz", "" }),
              "https://zoom.us/j/81132435236?pwd=xyz");
    EXPECT_EQ(findMeetingUrl({ "https://acme.zoom.us/j/123" }), "https://acme.zoom.us/j/123");
}

TEST(MeetingUrlTest, GoogleMeet)
{
    EXPECT_EQ(findMeetingUrl({ "Join with Google Meet: https://meet.google.com/abc-defg-hij\nhttps://tel.meet/abc-defg-hij?pin=1" }),
              "https://meet.google.com/abc-defg-hij");
}

TEST(MeetingUrlTest, LookalikeHostsAreIgnored)
{
    EXPECT_EQ(findMeetingUrl({ "https://notzoom.us/j/123 https://evil.com/teams.microsoft.com/l/meetup-join/x" }), "");
}
