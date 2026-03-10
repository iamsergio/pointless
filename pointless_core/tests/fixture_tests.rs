// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use pointless_core::{Data, Tag};

#[test]
fn test_load_test_json_fixture() {
    let json = include_str!("fixtures/test.json");
    let data = Data::from_json(json).expect("Failed to parse test.json");

    assert_eq!(data.revision(), 3);
    assert_eq!(data.task_count(), 7);
    assert_eq!(data.tag_count(), 3);

    let task1 = data.task_for_uuid("1").expect("Task 1 not found");
    assert_eq!(task1.title, "Task 1");
    assert_eq!(task1.revision, 2);
    assert!(!task1.is_done);
    assert!(!task1.is_important);
    assert_eq!(task1.tags, vec!["kde", "kdab"]);
    assert!(task1.parent_uuid.is_none());
    assert!(task1.modification_timestamp.is_none());
    assert!(task1.due_date.is_none());
    assert!(task1.completion_date.is_none());
    assert!(task1.last_pomodoro_date.is_none());
    assert!(task1.uuid_in_device_calendar.is_none());
    assert!(task1.device_calendar_uuid.is_none());
    assert!(task1.device_calendar_name.is_none());

    let task5 = data.task_for_uuid("5").expect("Task 5 not found");
    assert_eq!(task5.tags, vec!["current"]);
    assert!(task5.is_current());

    let task8 = data.task_for_uuid("8").expect("Task 8 not found");
    assert_eq!(task8.tags, vec!["soon"]);

    assert_eq!(data.get_tasks_by_tag("kde").len(), 2);
    assert_eq!(data.get_tasks_by_tag("kdab").len(), 2);
    assert_eq!(data.get_tasks_by_tag("current").len(), 1);
}

#[test]
fn test_load_test_null_values_fixture() {
    let json = include_str!("fixtures/test_null_values.json");
    let data = Data::from_json(json).expect("Failed to parse test_null_values.json");

    assert_eq!(data.revision(), 1);
    assert_eq!(data.task_count(), 2);
    assert_eq!(data.tag_count(), 0);

    let task2 = data.task_for_uuid("2").expect("Task 2 not found");
    assert_eq!(task2.title, "Task 2");
    assert_eq!(task2.revision, 2);
    assert!(task2.is_goal.is_none());
    assert!(task2.hide_on_weekends.is_none() || task2.hide_on_weekends == Some(false));

    let task3 = data.task_for_uuid("3").expect("Task 3 not found");
    assert_eq!(task3.title, "Task 3");
    assert!(task3.is_goal.is_none());
    assert!(task3.hide_on_weekends.is_none());
}

#[test]
fn test_fixture_roundtrip() {
    let json = include_str!("fixtures/test.json");
    let data = Data::from_json(json).expect("Failed to parse test.json");
    let serialized = data.to_json().expect("Failed to serialize");
    let data2 = Data::from_json(&serialized).expect("Failed to re-parse");

    assert_eq!(data2.revision(), data.revision());
    assert_eq!(data2.task_count(), data.task_count());
    assert_eq!(data2.tag_count(), data.tag_count());

    for i in 0..data.task_count() {
        let orig = data.task_at(i);
        let round = data2.task_for_uuid(&orig.uuid).expect("Task not found after roundtrip");
        assert_eq!(round.title, orig.title);
        assert_eq!(round.revision, orig.revision);
        assert_eq!(round.is_done, orig.is_done);
        assert_eq!(round.tags, orig.tags);
    }
}

#[test]
fn test_fixture_tag_deserialization() {
    let json = include_str!("fixtures/test.json");
    let data = Data::from_json(json).expect("Failed to parse test.json");

    let tags: Vec<Tag> = data.all_tags();
    assert_eq!(tags.len(), 3);

    let kde_tag = tags.iter().find(|t| t.name == "kde").expect("kde tag not found");
    assert_eq!(kde_tag.revision, 2);

    let soon_tag = tags.iter().find(|t| t.name == "soon").expect("soon tag not found");
    assert!(soon_tag.is_builtin());
}
