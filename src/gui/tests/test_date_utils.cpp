// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/date_utils.h"

#include <QDate>

#include <gtest/gtest.h>

TEST(DateUtilsTest, QDateRoundTrip)
{
    QDate originalDate(2026, 1, 15);
    ASSERT_TRUE(originalDate.isValid());

    auto timepoint = Gui::DateUtils::qdateToTimepoint(originalDate);
    ASSERT_TRUE(timepoint.has_value());

    QDate convertedDate = Gui::DateUtils::timepointToQDate(timepoint);
    EXPECT_EQ(originalDate, convertedDate);
}

TEST(DateUtilsTest, InvalidQDateReturnsNullopt)
{
    QDate invalidDate;
    ASSERT_FALSE(invalidDate.isValid());

    auto timepoint = Gui::DateUtils::qdateToTimepoint(invalidDate);
    EXPECT_FALSE(timepoint.has_value());
}

TEST(DateUtilsTest, NulloptTimepointReturnsInvalidQDate)
{
    std::optional<std::chrono::system_clock::time_point> nullTimepoint;

    QDate date = Gui::DateUtils::timepointToQDate(nullTimepoint);
    EXPECT_FALSE(date.isValid());
}

TEST(DateUtilsTest, QDateRoundTripMultipleDates)
{
    QDate dates[] = {
        QDate(2020, 1, 1),
        QDate(2025, 12, 31),
        QDate(2026, 2, 28),
        QDate(2024, 2, 29),
        QDate(1970, 1, 1),
        QDate(2100, 6, 15)
    };

    for (const auto &originalDate : dates) {
        ASSERT_TRUE(originalDate.isValid());

        auto timepoint = Gui::DateUtils::qdateToTimepoint(originalDate);
        ASSERT_TRUE(timepoint.has_value()) << "Failed for date: " << originalDate.toString().toStdString();

        QDate convertedDate = Gui::DateUtils::timepointToQDate(timepoint);
        EXPECT_EQ(originalDate, convertedDate) << "Failed for date: " << originalDate.toString().toStdString();
    }
}
