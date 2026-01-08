// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/calendarmodel.h"
#include "gui/Clock.h"
#include "gui/date_utils.h"

#include <QDate>

#include <gtest/gtest.h>

class CalendarModelTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QDateTime testTime(QDate(2026, 1, 15), QTime(12, 0));
        Gui::Clock::setTestNow(testTime);
    }

    void TearDown() override
    {
        Gui::Clock::reset();
    }
};

TEST_F(CalendarModelTest, RowCount)
{
    CalendarModel model;
    EXPECT_EQ(model.rowCount(), 42);
    EXPECT_EQ(model.count(), 42);
}

TEST_F(CalendarModelTest, InitialMonthIsToday)
{
    CalendarModel model;
    EXPECT_EQ(model.month(), QDate(2026, 1, 15));
}

TEST_F(CalendarModelTest, StartDateIsFirstMonday)
{
    CalendarModel model;
    const QDate startDate = model.startDate();
    EXPECT_TRUE(Gui::DateUtils::isMonday(startDate));
    EXPECT_EQ(startDate, QDate(2025, 12, 29)) << "Start date should be the first Monday before or on the 1st of the month" << startDate.toString().toStdString();
}

TEST_F(CalendarModelTest, DateRole)
{
    CalendarModel model;
    QDate startDate = model.startDate();

    for (int i = 0; i < 42; ++i) {
        QModelIndex index = model.index(i);
        QDate expectedDate = startDate.addDays(i);
        QDate actualDate = model.data(index, CalendarModel::DateRole).toDate();
        EXPECT_EQ(actualDate, expectedDate);
    }
}

TEST_F(CalendarModelTest, DayRole)
{
    CalendarModel model;
    QDate startDate = model.startDate();

    for (int i = 0; i < 42; ++i) {
        QModelIndex index = model.index(i);
        QDate expectedDate = startDate.addDays(i);
        int expectedDay = expectedDate.day();
        int actualDay = model.data(index, CalendarModel::DayRole).toInt();
        EXPECT_EQ(actualDay, expectedDay);
    }
}

TEST_F(CalendarModelTest, IsCurrentMonthRole)
{
    CalendarModel model;
    QDate startDate = model.startDate();
    int currentMonth = model.month().month();
    int currentYear = model.month().year();

    for (int i = 0; i < 42; ++i) {
        QModelIndex index = model.index(i);
        QDate date = startDate.addDays(i);
        bool expectedIsCurrentMonth = (date.month() == currentMonth && date.year() == currentYear);
        bool actualIsCurrentMonth = model.data(index, CalendarModel::IsCurrentMonthRole).toBool();
        EXPECT_EQ(actualIsCurrentMonth, expectedIsCurrentMonth);
    }
}

TEST_F(CalendarModelTest, IsTodayRole)
{
    CalendarModel model;
    QDate today = Gui::Clock::today();
    QDate startDate = model.startDate();

    bool foundToday = false;
    for (int i = 0; i < 42; ++i) {
        QModelIndex index = model.index(i);
        QDate date = startDate.addDays(i);
        bool expectedIsToday = (date == today);
        bool actualIsToday = model.data(index, CalendarModel::IsTodayRole).toBool();
        EXPECT_EQ(actualIsToday, expectedIsToday);
        if (actualIsToday) {
            foundToday = true;
        }
    }
    EXPECT_TRUE(foundToday);
}

TEST_F(CalendarModelTest, SetMonth)
{
    CalendarModel model;
    QDate newMonth(2026, 3, 10);
    model.setMonth(newMonth);

    EXPECT_EQ(model.month(), newMonth);

    QDate startDate = model.startDate();
    EXPECT_TRUE(Gui::DateUtils::isMonday(startDate));
}

TEST_F(CalendarModelTest, RoleNames)
{
    CalendarModel model;
    auto roleNames = model.roleNames();

    EXPECT_EQ(roleNames[CalendarModel::DateRole], "date");
    EXPECT_EQ(roleNames[CalendarModel::DayRole], "day");
    EXPECT_EQ(roleNames[CalendarModel::IsCurrentMonthRole], "isCurrentMonth");
    EXPECT_EQ(roleNames[CalendarModel::IsTodayRole], "isToday");
}
