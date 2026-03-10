// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use crate::calendar::DateRange;
use chrono::{DateTime, Datelike, Duration, NaiveDate, NaiveDateTime, NaiveTime, Utc};

#[derive(Debug, Clone)]
pub struct ICalEvent {
    pub uid: String,
    pub summary: String,
    pub dtstart: DateTime<Utc>,
    pub dtend: DateTime<Utc>,
    pub is_all_day: bool,
}

#[derive(Debug, Clone)]
enum Freq {
    Daily,
    Weekly,
    Monthly,
    Yearly,
}

#[derive(Debug, Clone)]
struct RRule {
    freq: Freq,
    count: Option<u32>,
    until: Option<DateTime<Utc>>,
    interval: u32,
}

struct RawVEvent {
    uid: String,
    summary: String,
    dtstart: DateTime<Utc>,
    dtend: DateTime<Utc>,
    is_all_day: bool,
    rrule: Option<RRule>,
}

fn parse_ical_datetime(value: &str) -> Option<(DateTime<Utc>, bool)> {
    let value = value.trim();

    if let Some(date_str) = value.strip_suffix('Z') {
        let ndt = NaiveDateTime::parse_from_str(date_str, "%Y%m%dT%H%M%S").ok()?;
        return Some((ndt.and_utc(), false));
    }

    if value.contains('T') {
        let ndt = NaiveDateTime::parse_from_str(value, "%Y%m%dT%H%M%S").ok()?;
        return Some((ndt.and_utc(), false));
    }

    let nd = NaiveDate::parse_from_str(value, "%Y%m%d").ok()?;
    let ndt = nd.and_time(NaiveTime::from_hms_opt(0, 0, 0)?);
    Some((ndt.and_utc(), true))
}

fn parse_dtstart_line(line: &str) -> Option<(DateTime<Utc>, bool)> {
    let after_colon = line.split(':').next_back()?;

    let is_date_value = line.contains("VALUE=DATE") && !line.contains("VALUE=DATE-TIME");

    let (dt, detected_all_day) = parse_ical_datetime(after_colon)?;
    Some((dt, is_date_value || detected_all_day))
}

fn parse_rrule(value: &str) -> Option<RRule> {
    let mut freq = None;
    let mut count = None;
    let mut until = None;
    let mut interval = 1u32;

    for part in value.split(';') {
        if let Some((key, val)) = part.split_once('=') {
            match key {
                "FREQ" => {
                    freq = match val {
                        "DAILY" => Some(Freq::Daily),
                        "WEEKLY" => Some(Freq::Weekly),
                        "MONTHLY" => Some(Freq::Monthly),
                        "YEARLY" => Some(Freq::Yearly),
                        _ => {
                            log::warn!("Unsupported RRULE FREQ: {}", val);
                            return None;
                        }
                    };
                }
                "COUNT" => {
                    count = val.parse().ok();
                }
                "UNTIL" => {
                    until = parse_ical_datetime(val).map(|(dt, _)| dt);
                }
                "INTERVAL" => {
                    interval = val.parse().unwrap_or(1);
                }
                _ => {}
            }
        }
    }

    Some(RRule {
        freq: freq?,
        count,
        until,
        interval,
    })
}

fn unfold_lines(ical_data: &str) -> String {
    ical_data
        .replace("\r\n ", "")
        .replace("\r\n\t", "")
        .replace("\n ", "")
        .replace("\n\t", "")
}

fn extract_vevents(data: &str) -> Vec<RawVEvent> {
    let mut events = Vec::new();
    let unfolded = unfold_lines(data);
    let lines: Vec<&str> = unfolded.lines().collect();

    let mut i = 0;
    while i < lines.len() {
        if lines[i].trim() == "BEGIN:VEVENT" {
            let mut uid = String::new();
            let mut summary = String::new();
            let mut dtstart: Option<(DateTime<Utc>, bool)> = None;
            let mut dtend: Option<DateTime<Utc>> = None;
            let mut rrule: Option<RRule> = None;
            let mut duration_str: Option<String> = None;

            i += 1;
            while i < lines.len() && lines[i].trim() != "END:VEVENT" {
                let line = lines[i].trim();
                if let Some(val) = strip_property(line, "UID") {
                    uid = val.to_string();
                } else if let Some(val) = strip_property(line, "SUMMARY") {
                    summary = val.to_string();
                } else if line.starts_with("DTSTART") {
                    dtstart = parse_dtstart_line(line);
                } else if line.starts_with("DTEND") {
                    dtend = parse_dtstart_line(line).map(|(dt, _)| dt);
                } else if let Some(val) = strip_property(line, "RRULE") {
                    rrule = parse_rrule(val);
                } else if let Some(val) = strip_property(line, "DURATION") {
                    duration_str = Some(val.to_string());
                }
                i += 1;
            }

            if let Some((start, is_all_day)) = dtstart {
                let end = if let Some(e) = dtend {
                    e
                } else if let Some(ref dur) = duration_str {
                    start + parse_ical_duration(dur).unwrap_or(Duration::hours(1))
                } else if is_all_day {
                    start + Duration::days(1)
                } else {
                    start + Duration::hours(1)
                };

                events.push(RawVEvent {
                    uid,
                    summary,
                    dtstart: start,
                    dtend: end,
                    is_all_day,
                    rrule,
                });
            }
        }
        i += 1;
    }

    events
}

fn strip_property<'a>(line: &'a str, name: &str) -> Option<&'a str> {
    if let Some(rest) = line.strip_prefix(name) {
        if let Some(stripped) = rest.strip_prefix(':') {
            return Some(stripped);
        }
    }
    None
}

fn parse_ical_duration(dur: &str) -> Option<Duration> {
    let s = dur.strip_prefix('P')?;
    let negative = s.starts_with('-');
    let s = if negative { &s[1..] } else { s };

    let mut total = Duration::zero();

    if let Some((date_part, time_part)) = s.split_once('T') {
        total += parse_duration_date_part(date_part);
        total += parse_duration_time_part(time_part);
    } else if let Some(time_part) = s.strip_prefix('T') {
        total += parse_duration_time_part(time_part);
    } else {
        total += parse_duration_date_part(s);
    }

    if negative {
        total = -total;
    }
    Some(total)
}

fn parse_duration_date_part(s: &str) -> Duration {
    let mut total = Duration::zero();
    let mut num_buf = String::new();
    for ch in s.chars() {
        if ch.is_ascii_digit() {
            num_buf.push(ch);
        } else {
            if let Ok(n) = num_buf.parse::<i64>() {
                match ch {
                    'D' => total += Duration::days(n),
                    'W' => total += Duration::weeks(n),
                    _ => {}
                }
            }
            num_buf.clear();
        }
    }
    total
}

fn parse_duration_time_part(s: &str) -> Duration {
    let mut total = Duration::zero();
    let mut num_buf = String::new();
    for ch in s.chars() {
        if ch.is_ascii_digit() {
            num_buf.push(ch);
        } else {
            if let Ok(n) = num_buf.parse::<i64>() {
                match ch {
                    'H' => total += Duration::hours(n),
                    'M' => total += Duration::minutes(n),
                    'S' => total += Duration::seconds(n),
                    _ => {}
                }
            }
            num_buf.clear();
        }
    }
    total
}

fn event_overlaps_range(start: &DateTime<Utc>, end: &DateTime<Utc>, range: &DateRange) -> bool {
    start < &range.end && end > &range.start
}

fn expand_rrule(
    raw: &RawVEvent,
    rrule: &RRule,
    range: &DateRange,
) -> Vec<ICalEvent> {
    let mut results = Vec::new();
    let event_duration = raw.dtend - raw.dtstart;
    let mut occurrence_start = raw.dtstart;
    let mut count = 0u32;

    let max_iterations = 10_000u32;
    let mut iterations = 0u32;

    loop {
        iterations += 1;
        if iterations > max_iterations {
            log::warn!(
                "RRULE expansion exceeded max iterations for event uid={}",
                raw.uid
            );
            break;
        }

        if let Some(until) = rrule.until {
            if occurrence_start > until {
                break;
            }
        }

        if let Some(max_count) = rrule.count {
            if count >= max_count {
                break;
            }
        }

        if occurrence_start > range.end {
            break;
        }

        let occurrence_end = occurrence_start + event_duration;

        if event_overlaps_range(&occurrence_start, &occurrence_end, range) {
            let occurrence_uid = format!(
                "{}_{}",
                raw.uid,
                occurrence_start.format("%Y%m%dT%H%M%SZ")
            );
            results.push(ICalEvent {
                uid: occurrence_uid,
                summary: raw.summary.clone(),
                dtstart: occurrence_start,
                dtend: occurrence_end,
                is_all_day: raw.is_all_day,
            });
        }

        count += 1;

        occurrence_start = advance_by_freq(occurrence_start, &rrule.freq, rrule.interval);
    }

    results
}

fn advance_by_freq(dt: DateTime<Utc>, freq: &Freq, interval: u32) -> DateTime<Utc> {
    match freq {
        Freq::Daily => dt + Duration::days(interval as i64),
        Freq::Weekly => dt + Duration::weeks(interval as i64),
        Freq::Monthly => {
            let naive = dt.naive_utc();
            let (year, month, day) = (naive.date().year(), naive.date().month(), naive.date().day());
            let total_months = (year * 12 + month as i32 - 1) + interval as i32;
            let new_year = total_months / 12;
            let new_month = (total_months % 12) as u32 + 1;
            let max_day = days_in_month(new_year, new_month);
            let new_day: u32 = day.min(max_day);
            let new_date = NaiveDate::from_ymd_opt(new_year, new_month, new_day)
                .unwrap_or(naive.date());
            new_date.and_time(naive.time()).and_utc()
        }
        Freq::Yearly => {
            let naive = dt.naive_utc();
            let new_year = naive.date().year() + interval as i32;
            let month = naive.date().month();
            let day = naive.date().day().min(days_in_month(new_year, month));
            let new_date = NaiveDate::from_ymd_opt(new_year, month, day)
                .unwrap_or(naive.date());
            new_date.and_time(naive.time()).and_utc()
        }
    }
}

fn days_in_month(year: i32, month: u32) -> u32 {
    match month {
        1 | 3 | 5 | 7 | 8 | 10 | 12 => 31,
        4 | 6 | 9 | 11 => 30,
        2 => {
            if (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) {
                29
            } else {
                28
            }
        }
        _ => 30,
    }
}

pub fn parse_ical_events(ical_data: &str, range: Option<&DateRange>) -> Vec<ICalEvent> {
    let raw_events = extract_vevents(ical_data);
    let mut results = Vec::new();

    for raw in &raw_events {
        if let Some(ref rrule) = raw.rrule {
            if let Some(range) = range {
                results.extend(expand_rrule(raw, rrule, range));
            } else {
                results.push(ICalEvent {
                    uid: raw.uid.clone(),
                    summary: raw.summary.clone(),
                    dtstart: raw.dtstart,
                    dtend: raw.dtend,
                    is_all_day: raw.is_all_day,
                });
            }
        } else {
            if let Some(range) = range {
                if !event_overlaps_range(&raw.dtstart, &raw.dtend, range) {
                    continue;
                }
            }
            results.push(ICalEvent {
                uid: raw.uid.clone(),
                summary: raw.summary.clone(),
                dtstart: raw.dtstart,
                dtend: raw.dtend,
                is_all_day: raw.is_all_day,
            });
        }
    }

    results
}

#[cfg(test)]
mod tests {
    use super::*;
    use chrono::TimeZone;

    #[test]
    fn test_parse_simple_event() {
        let ical = "\
BEGIN:VCALENDAR\r\n\
BEGIN:VEVENT\r\n\
UID:test-123\r\n\
SUMMARY:Team Meeting\r\n\
DTSTART:20250315T100000Z\r\n\
DTEND:20250315T110000Z\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        let events = parse_ical_events(ical, None);
        assert_eq!(events.len(), 1);
        assert_eq!(events[0].uid, "test-123");
        assert_eq!(events[0].summary, "Team Meeting");
        assert_eq!(events[0].dtstart, Utc.with_ymd_and_hms(2025, 3, 15, 10, 0, 0).unwrap());
        assert_eq!(events[0].dtend, Utc.with_ymd_and_hms(2025, 3, 15, 11, 0, 0).unwrap());
        assert!(!events[0].is_all_day);
    }

    #[test]
    fn test_parse_all_day_event() {
        let ical = "\
BEGIN:VCALENDAR\r\n\
BEGIN:VEVENT\r\n\
UID:allday-1\r\n\
SUMMARY:Holiday\r\n\
DTSTART;VALUE=DATE:20250401\r\n\
DTEND;VALUE=DATE:20250402\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        let events = parse_ical_events(ical, None);
        assert_eq!(events.len(), 1);
        assert!(events[0].is_all_day);
        assert_eq!(events[0].dtstart, Utc.with_ymd_and_hms(2025, 4, 1, 0, 0, 0).unwrap());
    }

    #[test]
    fn test_filter_by_range() {
        let ical = "\
BEGIN:VCALENDAR\r\n\
BEGIN:VEVENT\r\n\
UID:ev1\r\n\
SUMMARY:In Range\r\n\
DTSTART:20250310T100000Z\r\n\
DTEND:20250310T110000Z\r\n\
END:VEVENT\r\n\
BEGIN:VEVENT\r\n\
UID:ev2\r\n\
SUMMARY:Out of Range\r\n\
DTSTART:20250320T100000Z\r\n\
DTEND:20250320T110000Z\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        let range = DateRange {
            start: Utc.with_ymd_and_hms(2025, 3, 1, 0, 0, 0).unwrap(),
            end: Utc.with_ymd_and_hms(2025, 3, 15, 0, 0, 0).unwrap(),
        };

        let events = parse_ical_events(ical, Some(&range));
        assert_eq!(events.len(), 1);
        assert_eq!(events[0].uid, "ev1");
    }

    #[test]
    fn test_weekly_rrule() {
        let ical = "\
BEGIN:VCALENDAR\r\n\
BEGIN:VEVENT\r\n\
UID:weekly-1\r\n\
SUMMARY:Weekly Standup\r\n\
DTSTART:20250303T090000Z\r\n\
DTEND:20250303T093000Z\r\n\
RRULE:FREQ=WEEKLY;COUNT=4\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        let range = DateRange {
            start: Utc.with_ymd_and_hms(2025, 3, 1, 0, 0, 0).unwrap(),
            end: Utc.with_ymd_and_hms(2025, 4, 1, 0, 0, 0).unwrap(),
        };

        let events = parse_ical_events(ical, Some(&range));
        assert_eq!(events.len(), 4);
        assert_eq!(events[0].uid, "weekly-1_20250303T090000Z");
        assert_eq!(events[1].uid, "weekly-1_20250310T090000Z");
    }

    #[test]
    fn test_empty_input() {
        let events = parse_ical_events("", None);
        assert!(events.is_empty());
    }

    #[test]
    fn test_no_vevent() {
        let ical = "BEGIN:VCALENDAR\r\nEND:VCALENDAR\r\n";
        let events = parse_ical_events(ical, None);
        assert!(events.is_empty());
    }

    #[test]
    fn test_parse_ical_duration() {
        assert_eq!(parse_ical_duration("PT1H"), Some(Duration::hours(1)));
        assert_eq!(parse_ical_duration("PT30M"), Some(Duration::minutes(30)));
        assert_eq!(parse_ical_duration("P1D"), Some(Duration::days(1)));
        assert_eq!(parse_ical_duration("P1DT2H30M"), Some(Duration::days(1) + Duration::hours(2) + Duration::minutes(30)));
    }

    #[test]
    fn test_line_unfolding() {
        let ical = "BEGIN:VCALENDAR\r\nBEGIN:VEVENT\r\nUID:fold-test\r\nSUMMARY:A very long\r\n  summary line\r\nDTSTART:20250315T100000Z\r\nDTEND:20250315T110000Z\r\nEND:VEVENT\r\nEND:VCALENDAR\r\n";
        let events = parse_ical_events(ical, None);
        assert_eq!(events.len(), 1);
        assert_eq!(events[0].summary, "A very long summary line");
    }
}
