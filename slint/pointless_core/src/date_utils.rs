// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use chrono::{DateTime, Datelike, Duration, Local, NaiveTime, TimeZone, Timelike, Utc, Weekday};

use crate::clock;

pub fn trim_time(date: DateTime<Utc>) -> DateTime<Utc> {
    let local_date = date.with_timezone(&Local).date_naive();
    let midnight = NaiveTime::from_hms_opt(0, 0, 0).unwrap();
    let local_dt = local_date.and_time(midnight);
    Local
        .from_local_datetime(&local_dt)
        .single()
        .unwrap()
        .with_timezone(&Utc)
}

pub fn today() -> DateTime<Utc> {
    trim_time(clock::now())
}

pub fn is_today(date: DateTime<Utc>) -> bool {
    trim_time(date) == today()
}

pub fn is_tomorrow(date: DateTime<Utc>) -> bool {
    is_today(date - Duration::hours(24))
}

pub fn is_yesterday(date: DateTime<Utc>) -> bool {
    is_today(date + Duration::hours(24))
}

pub fn is_work_day(date: DateTime<Utc>) -> bool {
    let local = date.with_timezone(&Local);
    matches!(
        local.weekday(),
        Weekday::Mon | Weekday::Tue | Weekday::Wed | Weekday::Thu | Weekday::Fri
    )
}

pub fn is_midnight(date: DateTime<Utc>) -> bool {
    let local = date.with_timezone(&Local);
    local.hour() == 0 && local.minute() == 0 && local.second() == 0
}

pub fn this_weeks_monday(date: DateTime<Utc>) -> DateTime<Utc> {
    let local = date.with_timezone(&Local);
    let days_since_monday = local.weekday().num_days_from_monday();
    let monday = local.date_naive() - Duration::days(days_since_monday as i64);
    let midnight = NaiveTime::from_hms_opt(0, 0, 0).unwrap();
    Local
        .from_local_datetime(&monday.and_time(midnight))
        .single()
        .unwrap()
        .with_timezone(&Utc)
}

pub fn is_this_week(date: DateTime<Utc>) -> bool {
    let monday = this_weeks_monday(clock::now());
    date >= monday && date < monday + Duration::hours(24 * 7)
}

pub fn is_next_7_days(date: DateTime<Utc>) -> bool {
    let t = today();
    (is_today(date) || is_after_today(date)) && date < t + Duration::hours(24 * 7)
}

pub fn is_after_today(date: DateTime<Utc>) -> bool {
    trim_time(date) > today()
}

pub fn next_monday(date: DateTime<Utc>) -> DateTime<Utc> {
    let local = date.with_timezone(&Local);
    let days_since_monday = local.weekday().num_days_from_monday();
    let next_mon = local.date_naive() + Duration::days((7 - days_since_monday) as i64);
    let midnight = NaiveTime::from_hms_opt(0, 0, 0).unwrap();
    Local
        .from_local_datetime(&next_mon.and_time(midnight))
        .single()
        .unwrap()
        .with_timezone(&Utc)
}

pub fn weekday_name(date: DateTime<Utc>) -> &'static str {
    let local = date.with_timezone(&Local);
    match local.weekday() {
        Weekday::Mon => "Monday",
        Weekday::Tue => "Tuesday",
        Weekday::Wed => "Wednesday",
        Weekday::Thu => "Thursday",
        Weekday::Fri => "Friday",
        Weekday::Sat => "Saturday",
        Weekday::Sun => "Sunday",
    }
}

pub fn date_str(date: DateTime<Utc>) -> String {
    let local = date.with_timezone(&Local);
    format!("{}-{}-{}", local.day(), local.month(), local.year())
}

pub fn pretty_date(date: DateTime<Utc>, include_time: bool) -> String {
    if is_today(date) {
        return if include_time {
            format!("today {}", weekday_name(date))
        } else {
            "today".to_string()
        };
    }
    if is_tomorrow(date) {
        return "tomorrow".to_string();
    }
    if is_yesterday(date) {
        return "yesterday".to_string();
    }
    if is_this_week(date) {
        return format!("this {}", weekday_name(date));
    }
    if is_next_7_days(date) {
        return format!("next {}", weekday_name(date));
    }

    let mut result = date_str(date);
    if include_time {
        let local = date.with_timezone(&Local);
        result += &format!(" {:02}:{:02}", local.hour(), local.minute());
    }
    result
}

#[cfg(test)]
mod tests {
    use super::*;
    use chrono::TimeZone;

    fn make_utc(year: i32, month: u32, day: u32, hour: u32, min: u32, sec: u32) -> DateTime<Utc> {
        Utc.with_ymd_and_hms(year, month, day, hour, min, sec)
            .unwrap()
    }

    #[test]
    fn test_trim_time() {
        let dt = make_utc(2025, 6, 15, 14, 30, 45);
        let trimmed = trim_time(dt);
        let local = trimmed.with_timezone(&Local);
        assert_eq!(local.hour(), 0);
        assert_eq!(local.minute(), 0);
        assert_eq!(local.second(), 0);
    }

    #[test]
    fn test_is_today() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert!(is_today(make_utc(2025, 6, 15, 8, 0, 0)));
        assert!(!is_today(make_utc(2025, 6, 14, 8, 0, 0)));
        clock::reset();
    }

    #[test]
    fn test_is_tomorrow() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert!(is_tomorrow(make_utc(2025, 6, 16, 8, 0, 0)));
        assert!(!is_tomorrow(make_utc(2025, 6, 15, 8, 0, 0)));
        clock::reset();
    }

    #[test]
    fn test_is_yesterday() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert!(is_yesterday(make_utc(2025, 6, 14, 8, 0, 0)));
        assert!(!is_yesterday(make_utc(2025, 6, 15, 8, 0, 0)));
        clock::reset();
    }

    #[test]
    fn test_is_work_day() {
        // 2025-06-16 is a Monday
        assert!(is_work_day(make_utc(2025, 6, 16, 12, 0, 0)));
        // 2025-06-15 is a Sunday
        assert!(!is_work_day(make_utc(2025, 6, 15, 12, 0, 0)));
    }

    #[test]
    fn test_weekday_name() {
        // 2025-06-16 is a Monday
        assert_eq!(weekday_name(make_utc(2025, 6, 16, 12, 0, 0)), "Monday");
        // 2025-06-15 is a Sunday
        assert_eq!(weekday_name(make_utc(2025, 6, 15, 12, 0, 0)), "Sunday");
    }

    #[test]
    fn test_date_str() {
        let dt = make_utc(2025, 6, 15, 12, 0, 0);
        let s = date_str(dt);
        assert!(s.contains("2025"));
    }

    #[test]
    fn test_pretty_date_today() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert_eq!(pretty_date(make_utc(2025, 6, 15, 8, 0, 0), false), "today");
        clock::reset();
    }

    #[test]
    fn test_pretty_date_tomorrow() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert_eq!(
            pretty_date(make_utc(2025, 6, 16, 8, 0, 0), false),
            "tomorrow"
        );
        clock::reset();
    }

    #[test]
    fn test_pretty_date_yesterday() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert_eq!(
            pretty_date(make_utc(2025, 6, 14, 8, 0, 0), false),
            "yesterday"
        );
        clock::reset();
    }

    #[test]
    fn test_is_midnight() {
        let local_midnight = Local
            .from_local_datetime(
                &chrono::NaiveDate::from_ymd_opt(2025, 6, 15)
                    .unwrap()
                    .and_hms_opt(0, 0, 0)
                    .unwrap(),
            )
            .single()
            .unwrap()
            .with_timezone(&Utc);
        assert!(is_midnight(local_midnight));
    }

    #[test]
    fn test_this_weeks_monday() {
        // 2025-06-18 is a Wednesday
        let wed = make_utc(2025, 6, 18, 12, 0, 0);
        let monday = this_weeks_monday(wed);
        let local_monday = monday.with_timezone(&Local);
        assert_eq!(local_monday.weekday(), Weekday::Mon);
        assert_eq!(local_monday.day(), 16);
    }

    #[test]
    fn test_next_monday() {
        // 2025-06-18 is a Wednesday
        let wed = make_utc(2025, 6, 18, 12, 0, 0);
        let nm = next_monday(wed);
        let local_nm = nm.with_timezone(&Local);
        assert_eq!(local_nm.weekday(), Weekday::Mon);
        assert_eq!(local_nm.day(), 23);
    }

    #[test]
    fn test_is_after_today() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        assert!(is_after_today(make_utc(2025, 6, 16, 8, 0, 0)));
        assert!(!is_after_today(make_utc(2025, 6, 15, 8, 0, 0)));
        assert!(!is_after_today(make_utc(2025, 6, 14, 8, 0, 0)));
        clock::reset();
    }

    #[test]
    fn test_is_this_week() {
        // 2025-06-18 is a Wednesday, so Monday is 2025-06-16
        clock::set_test_now(make_utc(2025, 6, 18, 12, 0, 0));
        let monday = this_weeks_monday(clock::now());
        let next_mon = next_monday(clock::now());
        let sunday = monday + Duration::days(6);

        assert!(is_this_week(monday));
        assert!(is_this_week(sunday));
        assert!(!is_this_week(next_mon));
        clock::reset();
    }

    #[test]
    fn test_is_next_7_days() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        let now = clock::now();
        let yesterday = now - Duration::hours(24);
        let tomorrow = now + Duration::hours(24);
        let in_7_days = now + Duration::hours(24 * 7);

        assert!(is_next_7_days(now));
        assert!(!is_next_7_days(yesterday));
        assert!(is_next_7_days(tomorrow));
        assert!(!is_next_7_days(in_7_days));
        clock::reset();
    }
}
