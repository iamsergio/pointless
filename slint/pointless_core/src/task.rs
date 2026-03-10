// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use chrono::{DateTime, Duration, Utc};
use serde::{Deserialize, Serialize};

use crate::clock;
use crate::date_utils;
use crate::tag::{tag_is_builtin, BUILTIN_TAG_CURRENT, BUILTIN_TAG_EVENING, BUILTIN_TAG_SOON};

mod datetime_millis {
    use chrono::{DateTime, Utc};
    use serde::{self, Deserialize, Deserializer, Serializer};

    pub fn serialize<S>(date: &DateTime<Utc>, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        serializer.serialize_i64(date.timestamp_millis())
    }

    pub fn deserialize<'de, D>(deserializer: D) -> Result<DateTime<Utc>, D::Error>
    where
        D: Deserializer<'de>,
    {
        let millis = i64::deserialize(deserializer)?;
        DateTime::from_timestamp_millis(millis)
            .ok_or_else(|| serde::de::Error::custom("invalid timestamp"))
    }
}

mod option_datetime_millis {
    use chrono::{DateTime, Utc};
    use serde::{self, Deserialize, Deserializer, Serializer};

    pub fn serialize<S>(date: &Option<DateTime<Utc>>, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        match date {
            Some(d) => serializer.serialize_some(&d.timestamp_millis()),
            None => serializer.serialize_none(),
        }
    }

    pub fn deserialize<'de, D>(deserializer: D) -> Result<Option<DateTime<Utc>>, D::Error>
    where
        D: Deserializer<'de>,
    {
        let opt = Option::<i64>::deserialize(deserializer)?;
        match opt {
            Some(millis) => DateTime::from_timestamp_millis(millis)
                .map(Some)
                .ok_or_else(|| serde::de::Error::custom("invalid timestamp")),
            None => Ok(None),
        }
    }
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Task {
    #[serde(default = "default_revision")]
    pub revision: i32,

    #[serde(skip)]
    pub needs_sync_to_server: bool,

    pub uuid: String,

    #[serde(rename = "parentUuid")]
    pub parent_uuid: Option<String>,

    pub title: String,

    #[serde(rename = "isDone", default)]
    pub is_done: bool,

    #[serde(rename = "isGoal")]
    pub is_goal: Option<bool>,

    #[serde(rename = "isImportant", default)]
    pub is_important: bool,

    #[serde(rename = "hideOnWeekends")]
    pub hide_on_weekends: Option<bool>,

    #[serde(rename = "timesPerWeek", default = "default_times_per_week")]
    pub times_per_week: i32,

    #[serde(rename = "lastCompletions", default)]
    pub last_completions: Vec<i32>,

    #[serde(rename = "sectionName", default)]
    pub section_name: String,

    #[serde(default)]
    pub tags: Vec<String>,

    #[serde(rename = "creationTimestamp", with = "datetime_millis")]
    pub creation_timestamp: DateTime<Utc>,

    #[serde(rename = "modificationTimestamp", with = "option_datetime_millis")]
    pub modification_timestamp: Option<DateTime<Utc>>,

    #[serde(rename = "lastPomodoroDate", with = "option_datetime_millis")]
    pub last_pomodoro_date: Option<DateTime<Utc>>,

    #[serde(rename = "dueDate", with = "option_datetime_millis")]
    pub due_date: Option<DateTime<Utc>>,

    #[serde(rename = "completionDate", with = "option_datetime_millis")]
    pub completion_date: Option<DateTime<Utc>>,

    #[serde(rename = "uuidInDeviceCalendar")]
    pub uuid_in_device_calendar: Option<String>,

    #[serde(rename = "deviceCalendarUuid")]
    pub device_calendar_uuid: Option<String>,

    #[serde(rename = "deviceCalendarName")]
    pub device_calendar_name: Option<String>,

    pub description: Option<String>,
}

fn default_revision() -> i32 {
    -1
}

fn default_times_per_week() -> i32 {
    1
}

impl Default for Task {
    fn default() -> Self {
        Self {
            revision: -1,
            needs_sync_to_server: false,
            uuid: String::new(),
            parent_uuid: None,
            title: String::new(),
            is_done: false,
            is_goal: None,
            is_important: false,
            hide_on_weekends: None,
            times_per_week: 1,
            last_completions: Vec::new(),
            section_name: String::new(),
            tags: Vec::new(),
            creation_timestamp: DateTime::<Utc>::UNIX_EPOCH,
            modification_timestamp: None,
            last_pomodoro_date: None,
            due_date: None,
            completion_date: None,
            uuid_in_device_calendar: None,
            device_calendar_uuid: None,
            device_calendar_name: None,
            description: None,
        }
    }
}

impl Task {
    pub fn new(uuid: String, creation_timestamp: DateTime<Utc>, title: String) -> Self {
        Self {
            uuid,
            creation_timestamp,
            title,
            ..Default::default()
        }
    }

    pub fn contains_tag(&self, tag_name: &str) -> bool {
        self.tags.iter().any(|tag| tag == tag_name)
    }

    pub fn is_soon(&self) -> bool {
        if self.is_current() {
            return false;
        }
        self.contains_tag(BUILTIN_TAG_SOON) || self.is_due_in(Duration::days(15))
    }

    pub fn is_later(&self) -> bool {
        !self.is_soon() && !self.is_current()
    }

    pub fn is_current(&self) -> bool {
        self.contains_tag(BUILTIN_TAG_CURRENT) || self.is_due_this_week() || self.is_overdue()
    }

    pub fn is_evening(&self) -> bool {
        self.contains_tag(BUILTIN_TAG_EVENING)
    }

    pub fn tag_name(&self) -> String {
        for tag in &self.tags {
            if !tag_is_builtin(tag) {
                return tag.clone();
            }
        }
        String::new()
    }

    pub fn is_due_in(&self, days: Duration) -> bool {
        match self.due_date {
            None => false,
            Some(due) => {
                let now = clock::now();
                due <= now + days && due >= now
            }
        }
    }

    pub fn is_due_tomorrow(&self) -> bool {
        match self.due_date {
            None => false,
            Some(due) => date_utils::is_tomorrow(due),
        }
    }

    pub fn is_due_this_week(&self) -> bool {
        match self.due_date {
            None => false,
            Some(due) => date_utils::is_this_week(due),
        }
    }

    pub fn is_overdue(&self) -> bool {
        match self.due_date {
            None => false,
            Some(due) => {
                if self.is_done {
                    return false;
                }
                due < clock::now()
            }
        }
    }

    pub fn should_be_cleaned_up(&self) -> bool {
        let two_weeks_ago = clock::now() - Duration::days(14);
        self.is_done
            && match self.modification_timestamp {
                Some(mod_ts) => mod_ts < two_weeks_ago,
                None => true,
            }
    }

    pub fn set_tags(&mut self, new_tags: &[String]) {
        self.tags.clear();
        for tag in new_tags {
            self.add_tag(tag);
        }
    }

    pub fn add_tag(&mut self, tag: &str) -> bool {
        if !tag.is_empty() && !self.contains_tag(tag) {
            self.tags.push(tag.to_string());
            return true;
        }
        false
    }

    pub fn remove_builtin_tags(&mut self) {
        self.tags.retain(|tag| !tag_is_builtin(tag));
    }

    pub fn merge_conflict(&mut self, other: &Task) {
        self.is_done = self.is_done && other.is_done;
        self.is_important = self.is_important || other.is_important;

        let my_time = self
            .modification_timestamp
            .unwrap_or(DateTime::<Utc>::UNIX_EPOCH);
        let other_time = other
            .modification_timestamp
            .unwrap_or(DateTime::<Utc>::UNIX_EPOCH);
        let other_is_more_recent = other_time > my_time;

        match (self.due_date, other.due_date) {
            (Some(_), Some(_)) => {
                if other_is_more_recent {
                    self.due_date = other.due_date;
                }
            }
            (None, Some(_)) => {
                self.due_date = other.due_date;
            }
            _ => {}
        }

        if other_is_more_recent && other.title != self.title {
            self.title = other.title.clone();
        }

        match (&self.description, &other.description) {
            (Some(_), Some(_)) => {
                if other_is_more_recent {
                    self.description = other.description.clone();
                }
            }
            (None, Some(_)) => {
                self.description = other.description.clone();
            }
            _ => {}
        }

        let mut new_tags: Vec<String> = self.tags.clone();
        for tag in &other.tags {
            new_tags.push(tag.clone());
        }
        self.set_tags(&new_tags);

        let has_current = self.contains_tag(BUILTIN_TAG_CURRENT);
        let has_soon = self.contains_tag(BUILTIN_TAG_SOON);
        if has_current && has_soon {
            self.tags.retain(|tag| tag != BUILTIN_TAG_SOON);
        }
    }
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
    fn test_default() {
        let task = Task::default();
        assert_eq!(task.revision, -1);
        assert_eq!(task.times_per_week, 1);
        assert!(!task.is_done);
        assert!(!task.is_important);
        assert!(task.tags.is_empty());
    }

    #[test]
    fn test_new() {
        let ts = make_utc(2025, 6, 15, 12, 0, 0);
        let task = Task::new("abc".to_string(), ts, "My task".to_string());
        assert_eq!(task.uuid, "abc");
        assert_eq!(task.title, "My task");
        assert_eq!(task.creation_timestamp, ts);
        assert_eq!(task.revision, -1);
    }

    #[test]
    fn test_contains_tag() {
        let mut task = Task::default();
        task.tags = vec!["foo".to_string(), "bar".to_string()];
        assert!(task.contains_tag("foo"));
        assert!(!task.contains_tag("baz"));
    }

    #[test]
    fn test_add_tag() {
        let mut task = Task::default();
        assert!(task.add_tag("foo"));
        assert!(task.contains_tag("foo"));
        assert!(!task.add_tag("foo"));
        assert!(!task.add_tag(""));
    }

    #[test]
    fn test_set_tags_deduplicates() {
        let mut task = Task::default();
        task.set_tags(&["a".to_string(), "b".to_string(), "a".to_string()]);
        assert_eq!(task.tags.len(), 2);
    }

    #[test]
    fn test_remove_builtin_tags() {
        let mut task = Task::default();
        task.tags = vec![
            "soon".to_string(),
            "custom".to_string(),
            "current".to_string(),
        ];
        task.remove_builtin_tags();
        assert_eq!(task.tags, vec!["custom".to_string()]);
    }

    #[test]
    fn test_tag_name_returns_first_non_builtin() {
        let mut task = Task::default();
        task.tags = vec!["soon".to_string(), "myproject".to_string()];
        assert_eq!(task.tag_name(), "myproject");
    }

    #[test]
    fn test_tag_name_empty_when_all_builtin() {
        let mut task = Task::default();
        task.tags = vec!["soon".to_string(), "current".to_string()];
        assert_eq!(task.tag_name(), "");
    }

    #[test]
    fn test_is_evening() {
        let mut task = Task::default();
        assert!(!task.is_evening());
        task.tags.push("evening".to_string());
        assert!(task.is_evening());
    }

    #[test]
    fn test_is_overdue() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        let mut task = Task::default();
        task.due_date = Some(make_utc(2025, 6, 10, 12, 0, 0));
        assert!(task.is_overdue());
        task.is_done = true;
        assert!(!task.is_overdue());
        clock::reset();
    }

    #[test]
    fn test_is_due_in() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        let mut task = Task::default();
        task.due_date = Some(make_utc(2025, 6, 20, 12, 0, 0));
        assert!(task.is_due_in(Duration::days(15)));
        assert!(!task.is_due_in(Duration::days(1)));
        clock::reset();
    }

    #[test]
    fn test_should_be_cleaned_up() {
        clock::set_test_now(make_utc(2025, 6, 15, 12, 0, 0));
        let mut task = Task::default();
        task.is_done = true;
        assert!(task.should_be_cleaned_up());

        task.modification_timestamp = Some(make_utc(2025, 6, 14, 12, 0, 0));
        assert!(!task.should_be_cleaned_up());

        task.modification_timestamp = Some(make_utc(2025, 5, 1, 12, 0, 0));
        assert!(task.should_be_cleaned_up());

        task.is_done = false;
        assert!(!task.should_be_cleaned_up());
        clock::reset();
    }

    #[test]
    fn test_merge_conflict_is_done() {
        let mut a = Task::default();
        a.is_done = true;
        let mut b = Task::default();
        b.is_done = false;
        a.merge_conflict(&b);
        assert!(!a.is_done);
    }

    #[test]
    fn test_merge_conflict_is_important() {
        let mut a = Task::default();
        a.is_important = false;
        let mut b = Task::default();
        b.is_important = true;
        a.merge_conflict(&b);
        assert!(a.is_important);
    }

    #[test]
    fn test_merge_conflict_tags_deduplicated() {
        let mut a = Task::default();
        a.tags = vec!["foo".to_string()];
        let mut b = Task::default();
        b.tags = vec!["foo".to_string(), "bar".to_string()];
        a.merge_conflict(&b);
        assert!(a.contains_tag("foo"));
        assert!(a.contains_tag("bar"));
        assert_eq!(a.tags.len(), 2);
    }

    #[test]
    fn test_merge_conflict_current_removes_soon() {
        let mut a = Task::default();
        a.tags = vec!["current".to_string()];
        let mut b = Task::default();
        b.tags = vec!["soon".to_string()];
        a.merge_conflict(&b);
        assert!(a.contains_tag("current"));
        assert!(!a.contains_tag("soon"));
    }

    #[test]
    fn test_serde_roundtrip() {
        let ts = make_utc(2025, 6, 15, 12, 0, 0);
        let mut task = Task::new("uuid-1".to_string(), ts, "Test task".to_string());
        task.due_date = Some(make_utc(2025, 7, 1, 0, 0, 0));
        task.needs_sync_to_server = true;

        let json = serde_json::to_string(&task).unwrap();
        assert!(!json.contains("needsSyncToServer"));
        assert!(!json.contains("needs_sync_to_server"));

        let deserialized: Task = serde_json::from_str(&json).unwrap();
        assert_eq!(deserialized.uuid, "uuid-1");
        assert_eq!(deserialized.title, "Test task");
        assert_eq!(deserialized.creation_timestamp, ts);
        assert_eq!(deserialized.due_date, Some(make_utc(2025, 7, 1, 0, 0, 0)));
        assert!(!deserialized.needs_sync_to_server);
    }

    #[test]
    fn test_serde_timestamps_as_millis() {
        let ts = make_utc(2025, 6, 15, 12, 0, 0);
        let task = Task::new("uuid-1".to_string(), ts, "Test".to_string());
        let json = serde_json::to_string(&task).unwrap();
        let expected_millis = ts.timestamp_millis().to_string();
        assert!(json.contains(&expected_millis));
    }

    #[test]
    fn test_serde_null_optionals() {
        let task = Task::default();
        let json = serde_json::to_string(&task).unwrap();
        assert!(json.contains("\"dueDate\":null"));
        assert!(json.contains("\"modificationTimestamp\":null"));
    }

    #[test]
    fn test_is_due_this_week() {
        // January 12, 2026 is a Monday
        let monday = make_utc(2026, 1, 12, 0, 0, 0);
        let saturday = make_utc(2026, 1, 17, 0, 0, 0);
        let sunday = make_utc(2026, 1, 18, 0, 0, 0);
        let next_monday = make_utc(2026, 1, 19, 0, 0, 0);

        clock::set_test_now(monday);
        let mut task = Task::default();

        assert!(!task.is_due_this_week());

        task.due_date = Some(monday + Duration::hours(72));
        assert!(task.is_due_this_week());

        task.due_date = Some(sunday);
        assert!(task.is_due_this_week());

        task.due_date = Some(next_monday);
        assert!(!task.is_due_this_week());

        task.due_date = Some(monday - Duration::hours(24));
        assert!(!task.is_due_this_week());

        // From Saturday perspective
        clock::set_test_now(saturday);
        task.due_date = None;
        assert!(!task.is_due_this_week());

        task.due_date = Some(saturday + Duration::hours(12));
        assert!(task.is_due_this_week());

        task.due_date = Some(saturday + Duration::days(1));
        assert!(task.is_due_this_week());

        task.due_date = Some(saturday + Duration::days(2));
        assert!(!task.is_due_this_week());

        task.due_date = Some(sunday);
        assert!(task.is_due_this_week());

        task.due_date = Some(next_monday);
        assert!(!task.is_due_this_week());

        task.due_date = Some(saturday - Duration::hours(48));
        assert!(task.is_due_this_week());

        // From Sunday perspective
        clock::set_test_now(sunday);
        task.due_date = None;
        assert!(!task.is_due_this_week());

        task.due_date = Some(sunday + Duration::hours(12));
        assert!(task.is_due_this_week());

        task.due_date = Some(next_monday);
        assert!(!task.is_due_this_week());

        task.due_date = Some(next_monday + Duration::days(1));
        assert!(!task.is_due_this_week());

        task.due_date = Some(monday);
        assert!(task.is_due_this_week());

        clock::reset();
    }

    #[test]
    fn test_due_date_preserved_on_serialize_deserialize() {
        let mut task = Task::new(
            "uuid-dd".to_string(),
            make_utc(2025, 6, 15, 12, 0, 0),
            "Due date test".to_string(),
        );
        let due = make_utc(2025, 6, 20, 12, 0, 0);
        task.due_date = Some(due);

        let json = serde_json::to_string(&task).unwrap();
        assert!(json.contains("\"dueDate\""));

        let deserialized: Task = serde_json::from_str(&json).unwrap();
        assert!(deserialized.due_date.is_some());
        let diff = (due - deserialized.due_date.unwrap()).num_seconds().abs();
        assert!(diff < 2);
    }

    #[test]
    fn test_calendar_properties_serialize_deserialize() {
        let mut task = Task::default();
        task.uuid_in_device_calendar = Some("calendar-uuid-abc".to_string());
        task.device_calendar_uuid = Some("device-calendar-xyz".to_string());
        task.device_calendar_name = Some("Work Calendar".to_string());

        let json = serde_json::to_string(&task).unwrap();
        assert!(json.contains("\"uuidInDeviceCalendar\":\"calendar-uuid-abc\""));
        assert!(json.contains("\"deviceCalendarUuid\":\"device-calendar-xyz\""));
        assert!(json.contains("\"deviceCalendarName\":\"Work Calendar\""));

        let deserialized: Task = serde_json::from_str(&json).unwrap();
        assert_eq!(
            deserialized.uuid_in_device_calendar.as_deref(),
            Some("calendar-uuid-abc")
        );
        assert_eq!(
            deserialized.device_calendar_uuid.as_deref(),
            Some("device-calendar-xyz")
        );
        assert_eq!(
            deserialized.device_calendar_name.as_deref(),
            Some("Work Calendar")
        );
    }

    #[test]
    fn test_merge_conflict_due_date_both_have_due_date_more_recent_wins() {
        let now = make_utc(2025, 6, 15, 12, 0, 0);
        let one_hour_ago = now - Duration::hours(1);
        let two_hours_ago = now - Duration::hours(2);

        let mut task1 = Task::default();
        task1.due_date = Some(now + Duration::days(5));
        task1.modification_timestamp = Some(two_hours_ago);
        let mut task2 = Task::default();
        task2.due_date = Some(now + Duration::days(10));
        task2.modification_timestamp = Some(one_hour_ago);

        task1.merge_conflict(&task2);
        assert_eq!(task1.due_date, Some(now + Duration::days(10)));

        // Reverse: task with more recent modification keeps its own
        let mut task3 = Task::default();
        task3.due_date = Some(now + Duration::days(5));
        task3.modification_timestamp = Some(one_hour_ago);
        let mut task4 = Task::default();
        task4.due_date = Some(now + Duration::days(10));
        task4.modification_timestamp = Some(two_hours_ago);

        task3.merge_conflict(&task4);
        assert_eq!(task3.due_date, Some(now + Duration::days(5)));
    }

    #[test]
    fn test_merge_conflict_due_date_only_other_has_due_date() {
        let now = make_utc(2025, 6, 15, 12, 0, 0);

        let mut task1 = Task::default();
        task1.due_date = None;
        let mut task2 = Task::default();
        task2.due_date = Some(now + Duration::days(7));

        task1.merge_conflict(&task2);
        assert!(task1.due_date.is_some());
        assert_eq!(task1.due_date, Some(now + Duration::days(7)));
    }

    #[test]
    fn test_merge_conflict_due_date_only_this_has_due_date() {
        let now = make_utc(2025, 6, 15, 12, 0, 0);

        let mut task1 = Task::default();
        task1.due_date = Some(now + Duration::days(7));
        let task2 = Task::default();

        task1.merge_conflict(&task2);
        assert!(task1.due_date.is_some());
        assert_eq!(task1.due_date, Some(now + Duration::days(7)));
    }

    #[test]
    fn test_merge_conflict_title_more_recent_wins() {
        let now = make_utc(2025, 6, 15, 12, 0, 0);
        let one_hour_ago = now - Duration::hours(1);
        let two_hours_ago = now - Duration::hours(2);

        let mut task1 = Task::default();
        task1.title = "Old Title".to_string();
        task1.modification_timestamp = Some(two_hours_ago);
        let mut task2 = Task::default();
        task2.title = "New Title".to_string();
        task2.modification_timestamp = Some(one_hour_ago);

        task1.merge_conflict(&task2);
        assert_eq!(task1.title, "New Title");

        // Reverse
        let mut task3 = Task::default();
        task3.title = "Newer Title".to_string();
        task3.modification_timestamp = Some(one_hour_ago);
        let mut task4 = Task::default();
        task4.title = "Older Title".to_string();
        task4.modification_timestamp = Some(two_hours_ago);

        task3.merge_conflict(&task4);
        assert_eq!(task3.title, "Newer Title");
    }

    #[test]
    fn test_merge_conflict_title_same_title_no_change() {
        let now = make_utc(2025, 6, 15, 12, 0, 0);

        let mut task1 = Task::default();
        task1.title = "Same Title".to_string();
        task1.modification_timestamp = Some(now - Duration::hours(2));
        let mut task2 = Task::default();
        task2.title = "Same Title".to_string();
        task2.modification_timestamp = Some(now - Duration::hours(1));

        task1.merge_conflict(&task2);
        assert_eq!(task1.title, "Same Title");
    }

    #[test]
    fn test_merge_conflict_tags_no_duplicates() {
        let mut task1 = Task::default();
        task1.tags = vec!["tag1".to_string(), "tag2".to_string()];
        let mut task2 = Task::default();
        task2.tags = vec!["tag1".to_string(), "tag2".to_string()];

        task1.merge_conflict(&task2);
        assert_eq!(task1.tags.len(), 2);
    }

    #[test]
    fn test_merge_conflict_current_wins_over_soon_both_on_this() {
        let mut task1 = Task::default();
        task1.tags = vec!["current".to_string(), "soon".to_string()];
        let task2 = Task::default();

        task1.merge_conflict(&task2);
        assert!(task1.contains_tag("current"));
        assert!(!task1.contains_tag("soon"));
    }

    #[test]
    fn test_merge_conflict_current_wins_over_soon_both_on_other() {
        let mut task1 = Task::default();
        let mut task2 = Task::default();
        task2.tags = vec!["current".to_string(), "soon".to_string()];

        task1.merge_conflict(&task2);
        assert!(task1.contains_tag("current"));
        assert!(!task1.contains_tag("soon"));
    }

    #[test]
    fn test_is_due_tomorrow() {
        // January 17, 2026
        let now = make_utc(2026, 1, 17, 0, 0, 0);
        clock::set_test_now(now);

        let mut task = Task::default();

        // No due date
        assert!(!task.is_due_tomorrow());

        // Due today
        task.due_date = Some(now);
        assert!(!task.is_due_tomorrow());

        // Due tomorrow
        task.due_date = Some(now + Duration::days(1));
        assert!(task.is_due_tomorrow());

        // Due day after tomorrow
        task.due_date = Some(now + Duration::days(2));
        assert!(!task.is_due_tomorrow());

        // Due yesterday
        task.due_date = Some(now - Duration::days(1));
        assert!(!task.is_due_tomorrow());

        clock::reset();
    }
}
