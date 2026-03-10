// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use std::cell::Cell;
use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use crate::error::PointlessError;
use crate::tag::Tag;
use crate::task::Task;

fn default_revision() -> i32 {
    -1
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct DataPayload {
    #[serde(default = "default_revision")]
    pub revision: i32,

    #[serde(default)]
    pub tasks: Vec<Task>,

    #[serde(default)]
    pub tags: Vec<Tag>,

    #[serde(rename = "deletedTaskUuids", default)]
    pub deleted_task_uuids: Vec<String>,

    #[serde(rename = "deletedTagNames", default)]
    pub deleted_tag_names: Vec<String>,
}

impl Default for DataPayload {
    fn default() -> Self {
        Self {
            revision: -1,
            tasks: Vec::new(),
            tags: Vec::new(),
            deleted_task_uuids: Vec::new(),
            deleted_tag_names: Vec::new(),
        }
    }
}

pub struct Data {
    pub payload: DataPayload,
    pub needs_upload: Cell<bool>,
    pub needs_local_save: Cell<bool>,
}

impl Default for Data {
    fn default() -> Self {
        Self::new()
    }
}

impl Data {
    pub fn new() -> Self {
        Self {
            payload: DataPayload::default(),
            needs_upload: Cell::new(false),
            needs_local_save: Cell::new(false),
        }
    }

    // --- Task management ---

    pub fn add_task(&mut self, task: Task) {
        if self.task_for_uuid(&task.uuid).is_some() {
            return;
        }
        self.payload.tasks.push(task);
    }

    pub fn remove_task(&mut self, uuid: &str) -> bool {
        let len_before = self.payload.tasks.len();
        self.payload.tasks.retain(|t| t.uuid != uuid);
        self.payload.tasks.len() != len_before
    }

    pub fn get_task(&self, uuid: &str) -> Option<Task> {
        self.task_for_uuid(uuid).cloned()
    }

    pub fn get_all_tasks(&self) -> Vec<Task> {
        self.payload.tasks.clone()
    }

    pub fn update_task(&mut self, task: Task, increment_task_revision: bool) -> bool {
        if let Some(existing) = self.task_for_uuid_mut(&task.uuid) {
            *existing = task;
            if increment_task_revision {
                existing.revision += 1;
            }
            true
        } else {
            false
        }
    }

    pub fn set_task(&mut self, task: Task) -> bool {
        if let Some(existing) = self.task_for_uuid_mut(&task.uuid) {
            *existing = task;
            true
        } else {
            false
        }
    }

    pub fn clear_tasks(&mut self) {
        self.payload.tasks.clear();
    }

    pub fn task_count(&self) -> usize {
        self.payload.tasks.len()
    }

    pub fn new_tasks(&self) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| t.revision == -1)
            .cloned()
            .collect()
    }

    pub fn modified_tasks(&self) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| t.needs_sync_to_server)
            .cloned()
            .collect()
    }

    // --- Task filtering ---

    pub fn get_tasks_by_tag(&self, tag_name: &str) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| t.contains_tag(tag_name))
            .cloned()
            .collect()
    }

    pub fn get_completed_tasks(&self) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| t.is_done)
            .cloned()
            .collect()
    }

    pub fn get_pending_tasks(&self) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| !t.is_done)
            .cloned()
            .collect()
    }

    pub fn get_important_tasks(&self) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| t.is_important)
            .cloned()
            .collect()
    }

    pub fn get_tasks_by_parent(&self, parent_uuid: &str) -> Vec<Task> {
        self.payload
            .tasks
            .iter()
            .filter(|t| t.parent_uuid.as_deref() == Some(parent_uuid))
            .cloned()
            .collect()
    }

    pub fn task_at(&self, index: usize) -> &Task {
        &self.payload.tasks[index]
    }

    pub fn task_for_uuid(&self, uuid: &str) -> Option<&Task> {
        self.payload.tasks.iter().find(|t| t.uuid == uuid)
    }

    pub fn task_for_uuid_mut(&mut self, uuid: &str) -> Option<&mut Task> {
        self.payload.tasks.iter_mut().find(|t| t.uuid == uuid)
    }

    pub fn task_for_title(&self, title: &str) -> Option<&Task> {
        self.payload.tasks.iter().find(|t| t.title == title)
    }

    pub fn debug_task_uids(&self) -> String {
        self.payload
            .tasks
            .iter()
            .map(|t| t.uuid.as_str())
            .collect::<Vec<_>>()
            .join(", ")
    }

    pub fn find_duplicate_calendar_task_uuids(&self) -> Vec<String> {
        let mut groups: HashMap<(String, String, Option<i64>), Vec<&Task>> = HashMap::new();

        for task in &self.payload.tasks {
            if task.device_calendar_name.is_none() {
                continue;
            }

            let key = (
                task.title.clone(),
                task.device_calendar_name.clone().unwrap_or_default(),
                task.due_date.map(|d| d.timestamp_millis()),
            );

            groups.entry(key).or_default().push(task);
        }

        let mut duplicate_uuids = Vec::new();
        for tasks in groups.values() {
            if tasks.len() <= 1 {
                continue;
            }

            let most_recent = tasks
                .iter()
                .max_by_key(|t| t.modification_timestamp)
                .unwrap();

            for task in tasks {
                if task.uuid != most_recent.uuid {
                    duplicate_uuids.push(task.uuid.clone());
                }
            }
        }

        duplicate_uuids
    }

    // --- Tag management ---

    pub fn add_tag(&mut self, tag: Tag) {
        if !self.contains_tag(&tag.name) {
            self.payload.tags.push(tag);
        }
    }

    pub fn remove_tag(&mut self, tag_name: &str) -> bool {
        let len_before = self.payload.tags.len();
        self.payload.tags.retain(|t| t.name != tag_name);
        self.payload.tags.len() != len_before
    }

    pub fn get_tag(&self, tag_name: &str) -> Option<Tag> {
        self.payload
            .tags
            .iter()
            .find(|t| t.name == tag_name)
            .cloned()
    }

    pub fn all_tags(&self) -> Vec<Tag> {
        self.payload.tags.clone()
    }

    pub fn clear_tags(&mut self) {
        self.payload.tags.clear();
    }

    pub fn tag_count(&self) -> usize {
        self.payload.tags.len()
    }

    pub fn new_tags(&self) -> Vec<Tag> {
        self.payload
            .tags
            .iter()
            .filter(|t| t.revision == -1)
            .cloned()
            .collect()
    }

    pub fn tag_at(&self, index: usize) -> &Tag {
        &self.payload.tags[index]
    }

    pub fn contains_tag(&self, tag_name: &str) -> bool {
        self.payload.tags.iter().any(|t| t.name == tag_name)
    }

    pub fn get_used_tags(&self) -> Vec<Tag> {
        self.payload
            .tags
            .iter()
            .filter(|tag| {
                self.payload
                    .tasks
                    .iter()
                    .any(|task| task.contains_tag(&tag.name))
            })
            .cloned()
            .collect()
    }

    pub fn get_unused_tags(&self) -> Vec<Tag> {
        self.payload
            .tags
            .iter()
            .filter(|tag| {
                !self
                    .payload
                    .tasks
                    .iter()
                    .any(|task| task.contains_tag(&tag.name))
            })
            .cloned()
            .collect()
    }

    pub fn remove_unused_tags(&mut self) {
        let unused_names: Vec<String> = self
            .get_unused_tags()
            .iter()
            .map(|t| t.name.clone())
            .collect();
        self.payload
            .tags
            .retain(|t| !unused_names.contains(&t.name));
    }

    // --- Deleted items ---

    pub fn add_deleted_task_uuid(&mut self, uuid: String) {
        self.payload.deleted_task_uuids.push(uuid);
    }

    pub fn deleted_task_uuids(&self) -> &[String] {
        &self.payload.deleted_task_uuids
    }

    pub fn add_deleted_tag_name(&mut self, tag_name: String) {
        self.payload.deleted_tag_names.push(tag_name);
    }

    pub fn deleted_tag_names(&self) -> &[String] {
        &self.payload.deleted_tag_names
    }

    // --- Sync ---

    pub fn clear_server_sync_bits(&mut self) {
        if self.payload.revision == -1 {
            self.payload.revision = 0;
        }

        for task in &mut self.payload.tasks {
            task.needs_sync_to_server = false;
            if task.revision == -1 {
                task.revision = 0;
            }
        }

        for tag in &mut self.payload.tags {
            tag.needs_sync_to_server = false;
            if tag.revision == -1 {
                tag.revision = 0;
            }
        }

        self.payload.deleted_task_uuids.clear();
        self.payload.deleted_tag_names.clear();
        self.needs_upload.set(false);
        self.needs_local_save.set(false);
    }

    pub fn set_revision(&mut self, revision: i32) {
        self.payload.revision = revision;
    }

    pub fn revision(&self) -> i32 {
        self.payload.revision
    }

    // --- Serialization ---

    pub fn from_json(json: &str) -> Result<Data, PointlessError> {
        let payload: DataPayload = serde_json::from_str(json)?;
        Ok(Data {
            payload,
            needs_upload: Cell::new(false),
            needs_local_save: Cell::new(false),
        })
    }

    pub fn to_json(&self) -> Result<String, PointlessError> {
        let json = serde_json::to_string(&self.payload)?;
        Ok(json)
    }

    pub fn is_empty(&self) -> bool {
        self.payload.tasks.is_empty() && self.payload.tags.is_empty()
    }

    pub fn is_valid(&self) -> bool {
        self.payload.revision != -1 || !self.is_empty()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use chrono::{DateTime, TimeZone, Utc};

    fn make_task(uuid: &str, title: &str) -> Task {
        Task::new(
            uuid.to_string(),
            Utc.with_ymd_and_hms(2025, 6, 15, 12, 0, 0).unwrap(),
            title.to_string(),
        )
    }

    #[test]
    fn test_add_task_prevents_duplicates() {
        let mut data = Data::new();
        let task = make_task("u1", "Task 1");
        data.add_task(task.clone());
        data.add_task(task);
        assert_eq!(data.task_count(), 1);
    }

    #[test]
    fn test_remove_task() {
        let mut data = Data::new();
        data.add_task(make_task("u1", "Task 1"));
        assert!(data.remove_task("u1"));
        assert!(!data.remove_task("u1"));
        assert_eq!(data.task_count(), 0);
    }

    #[test]
    fn test_get_task() {
        let mut data = Data::new();
        data.add_task(make_task("u1", "Task 1"));
        let t = data.get_task("u1");
        assert!(t.is_some());
        assert_eq!(t.unwrap().title, "Task 1");
        assert!(data.get_task("nonexistent").is_none());
    }

    #[test]
    fn test_update_task() {
        let mut data = Data::new();
        let mut task = make_task("u1", "Original");
        task.revision = 5;
        data.add_task(task);

        let mut updated = make_task("u1", "Updated");
        updated.revision = 5;
        assert!(data.update_task(updated, true));
        let t = data.get_task("u1").unwrap();
        assert_eq!(t.title, "Updated");
        assert_eq!(t.revision, 6);

        assert!(!data.update_task(make_task("nonexistent", "X"), false));
    }

    #[test]
    fn test_set_task() {
        let mut data = Data::new();
        data.add_task(make_task("u1", "Original"));
        let updated = make_task("u1", "Updated");
        assert!(data.set_task(updated));
        assert_eq!(data.get_task("u1").unwrap().title, "Updated");
        assert!(!data.set_task(make_task("nonexistent", "X")));
    }

    #[test]
    fn test_new_tasks_and_modified_tasks() {
        let mut data = Data::new();
        let mut t1 = make_task("u1", "New");
        t1.revision = -1;
        let mut t2 = make_task("u2", "Existing");
        t2.revision = 1;
        t2.needs_sync_to_server = true;
        let mut t3 = make_task("u3", "Clean");
        t3.revision = 2;
        data.add_task(t1);
        data.add_task(t2);
        data.add_task(t3);

        assert_eq!(data.new_tasks().len(), 1);
        assert_eq!(data.modified_tasks().len(), 1);
    }

    #[test]
    fn test_get_tasks_by_tag() {
        let mut data = Data::new();
        let mut t1 = make_task("u1", "T1");
        t1.tags = vec!["work".to_string()];
        let t2 = make_task("u2", "T2");
        data.add_task(t1);
        data.add_task(t2);
        assert_eq!(data.get_tasks_by_tag("work").len(), 1);
    }

    #[test]
    fn test_completed_pending_important() {
        let mut data = Data::new();
        let mut t1 = make_task("u1", "Done");
        t1.is_done = true;
        let mut t2 = make_task("u2", "Important");
        t2.is_important = true;
        let t3 = make_task("u3", "Normal");
        data.add_task(t1);
        data.add_task(t2);
        data.add_task(t3);

        assert_eq!(data.get_completed_tasks().len(), 1);
        assert_eq!(data.get_pending_tasks().len(), 2);
        assert_eq!(data.get_important_tasks().len(), 1);
    }

    #[test]
    fn test_task_for_title() {
        let mut data = Data::new();
        data.add_task(make_task("u1", "Find me"));
        assert!(data.task_for_title("Find me").is_some());
        assert!(data.task_for_title("Not here").is_none());
    }

    #[test]
    fn test_tags() {
        let mut data = Data::new();
        let tag = Tag {
            name: "work".to_string(),
            ..Default::default()
        };
        data.add_tag(tag.clone());
        data.add_tag(tag);
        assert_eq!(data.tag_count(), 1);

        assert!(data.contains_tag("work"));
        assert!(data.get_tag("work").is_some());
        assert!(data.remove_tag("work"));
        assert!(!data.remove_tag("work"));
        assert_eq!(data.tag_count(), 0);
    }

    #[test]
    fn test_used_unused_tags() {
        let mut data = Data::new();
        data.add_tag(Tag {
            name: "used".to_string(),
            ..Default::default()
        });
        data.add_tag(Tag {
            name: "unused".to_string(),
            ..Default::default()
        });
        let mut t = make_task("u1", "T1");
        t.tags = vec!["used".to_string()];
        data.add_task(t);

        assert_eq!(data.get_used_tags().len(), 1);
        assert_eq!(data.get_unused_tags().len(), 1);

        data.remove_unused_tags();
        assert_eq!(data.tag_count(), 1);
        assert!(data.contains_tag("used"));
    }

    #[test]
    fn test_deleted_items() {
        let mut data = Data::new();
        data.add_deleted_task_uuid("u1".to_string());
        data.add_deleted_tag_name("tag1".to_string());
        assert_eq!(data.deleted_task_uuids().len(), 1);
        assert_eq!(data.deleted_tag_names().len(), 1);
    }

    #[test]
    fn test_clear_server_sync_bits() {
        let mut data = Data::new();
        let mut t = make_task("u1", "T1");
        t.needs_sync_to_server = true;
        data.add_task(t);
        data.add_tag(Tag {
            name: "tag1".to_string(),
            ..Default::default()
        });
        data.add_deleted_task_uuid("del1".to_string());
        data.add_deleted_tag_name("deltag".to_string());
        data.needs_upload.set(true);
        data.needs_local_save.set(true);

        data.clear_server_sync_bits();

        assert_eq!(data.revision(), 0);
        assert!(!data.task_for_uuid("u1").unwrap().needs_sync_to_server);
        assert!(data.deleted_task_uuids().is_empty());
        assert!(data.deleted_tag_names().is_empty());
        assert!(!data.needs_upload.get());
        assert!(!data.needs_local_save.get());
    }

    #[test]
    fn test_is_empty_and_is_valid() {
        let data = Data::new();
        assert!(data.is_empty());
        assert!(!data.is_valid());

        let mut data2 = Data::new();
        data2.add_task(make_task("u1", "T1"));
        assert!(!data2.is_empty());
        assert!(data2.is_valid());

        let mut data3 = Data::new();
        data3.set_revision(1);
        assert!(data3.is_valid());
    }

    #[test]
    fn test_json_roundtrip() {
        let mut data = Data::new();
        data.set_revision(42);
        data.add_task(make_task("u1", "Task 1"));
        data.add_tag(Tag {
            name: "work".to_string(),
            ..Default::default()
        });
        data.add_deleted_task_uuid("del1".to_string());
        data.add_deleted_tag_name("deltag".to_string());

        let json = data.to_json().unwrap();
        assert!(json.contains("deletedTaskUuids"));
        assert!(json.contains("deletedTagNames"));

        let data2 = Data::from_json(&json).unwrap();
        assert_eq!(data2.revision(), 42);
        assert_eq!(data2.task_count(), 1);
        assert_eq!(data2.tag_count(), 1);
        assert_eq!(data2.deleted_task_uuids().len(), 1);
        assert_eq!(data2.deleted_tag_names().len(), 1);
    }

    #[test]
    fn test_json_ignores_unknown_fields() {
        let json = r#"{"revision":1,"tasks":[],"tags":[],"deletedTaskUuids":[],"deletedTagNames":[],"unknownField":true}"#;
        let result = Data::from_json(json);
        assert!(result.is_ok());
    }

    #[test]
    fn test_debug_task_uids() {
        let mut data = Data::new();
        data.add_task(make_task("a", "A"));
        data.add_task(make_task("b", "B"));
        let s = data.debug_task_uids();
        assert!(s.contains("a"));
        assert!(s.contains("b"));
    }

    #[test]
    fn test_find_duplicate_calendar_task_uuids_no_duplicates() {
        let mut data = Data::new();

        let mut t1 = make_task("uuid-1", "Meeting A");
        t1.uuid_in_device_calendar = Some("cal-event-1".to_string());
        t1.device_calendar_name = Some("Work".to_string());
        data.add_task(t1);

        let mut t2 = make_task("uuid-2", "Meeting B");
        t2.uuid_in_device_calendar = Some("cal-event-2".to_string());
        t2.device_calendar_name = Some("Work".to_string());
        data.add_task(t2);

        let dupes = data.find_duplicate_calendar_task_uuids();
        assert!(dupes.is_empty());
    }

    #[test]
    fn test_find_duplicate_calendar_task_uuids_returns_older_task() {
        let mut data = Data::new();
        let older = DateTime::from_timestamp_millis(1000).unwrap();
        let newer = DateTime::from_timestamp_millis(2000).unwrap();
        let event_date = DateTime::from_timestamp_millis(100 * 24 * 3600 * 1000).unwrap();

        let mut t1 = make_task("uuid-1", "Meeting A");
        t1.uuid_in_device_calendar = Some("ios-event-1".to_string());
        t1.device_calendar_name = Some("Work".to_string());
        t1.due_date = Some(event_date);
        t1.modification_timestamp = Some(older);
        data.add_task(t1);

        let mut t2 = make_task("uuid-2", "Meeting A");
        t2.uuid_in_device_calendar = Some("linux-event-1".to_string());
        t2.device_calendar_name = Some("Work".to_string());
        t2.due_date = Some(event_date);
        t2.modification_timestamp = Some(newer);
        data.add_task(t2);

        let dupes = data.find_duplicate_calendar_task_uuids();
        assert_eq!(dupes.len(), 1);
        assert_eq!(dupes[0], "uuid-1");
    }

    #[test]
    fn test_find_duplicate_calendar_task_uuids_missing_modification_timestamp() {
        let mut data = Data::new();
        let some_time = DateTime::from_timestamp_millis(5000).unwrap();
        let event_date = DateTime::from_timestamp_millis(100 * 24 * 3600 * 1000).unwrap();

        let mut t1 = make_task("uuid-1", "Meeting A");
        t1.uuid_in_device_calendar = Some("ios-event-1".to_string());
        t1.device_calendar_name = Some("Work".to_string());
        t1.due_date = Some(event_date);
        // no modification_timestamp
        data.add_task(t1);

        let mut t2 = make_task("uuid-2", "Meeting A");
        t2.uuid_in_device_calendar = Some("linux-event-1".to_string());
        t2.device_calendar_name = Some("Work".to_string());
        t2.due_date = Some(event_date);
        t2.modification_timestamp = Some(some_time);
        data.add_task(t2);

        let dupes = data.find_duplicate_calendar_task_uuids();
        assert_eq!(dupes.len(), 1);
        assert_eq!(dupes[0], "uuid-1");
    }

    #[test]
    fn test_find_duplicate_calendar_task_uuids_three_duplicates() {
        let mut data = Data::new();
        let t1_time = DateTime::from_timestamp_millis(1000).unwrap();
        let t2_time = DateTime::from_timestamp_millis(3000).unwrap();
        let t3_time = DateTime::from_timestamp_millis(2000).unwrap();
        let event_date = DateTime::from_timestamp_millis(100 * 24 * 3600 * 1000).unwrap();

        let mut t1 = make_task("uuid-1", "Meeting A");
        t1.uuid_in_device_calendar = Some("event-1".to_string());
        t1.device_calendar_name = Some("Work".to_string());
        t1.due_date = Some(event_date);
        t1.modification_timestamp = Some(t1_time);
        data.add_task(t1);

        let mut t2 = make_task("uuid-2", "Meeting A");
        t2.uuid_in_device_calendar = Some("event-2".to_string());
        t2.device_calendar_name = Some("Work".to_string());
        t2.due_date = Some(event_date);
        t2.modification_timestamp = Some(t2_time);
        data.add_task(t2);

        let mut t3 = make_task("uuid-3", "Meeting A");
        t3.uuid_in_device_calendar = Some("event-3".to_string());
        t3.device_calendar_name = Some("Work".to_string());
        t3.due_date = Some(event_date);
        t3.modification_timestamp = Some(t3_time);
        data.add_task(t3);

        let dupes = data.find_duplicate_calendar_task_uuids();
        assert_eq!(dupes.len(), 2);
        assert!(dupes.contains(&"uuid-1".to_string()));
        assert!(dupes.contains(&"uuid-3".to_string()));
        assert!(!dupes.contains(&"uuid-2".to_string()));
    }

    #[test]
    fn test_find_duplicate_calendar_task_uuids_recurring_events_not_deduplicated() {
        let mut data = Data::new();
        let monday = DateTime::from_timestamp_millis(100 * 24 * 3600 * 1000).unwrap();
        let tuesday = DateTime::from_timestamp_millis(101 * 24 * 3600 * 1000).unwrap();

        let mut t1 = make_task("uuid-1", "Standup");
        t1.uuid_in_device_calendar = Some("event-1".to_string());
        t1.device_calendar_name = Some("Work".to_string());
        t1.due_date = Some(monday);
        data.add_task(t1);

        let mut t2 = make_task("uuid-2", "Standup");
        t2.uuid_in_device_calendar = Some("event-2".to_string());
        t2.device_calendar_name = Some("Work".to_string());
        t2.due_date = Some(tuesday);
        data.add_task(t2);

        let dupes = data.find_duplicate_calendar_task_uuids();
        assert!(dupes.is_empty());
    }

    #[test]
    fn test_serialize_empty_deleted_lists() {
        let data = Data::new();
        let json = data.to_json().unwrap();
        let data2 = Data::from_json(&json).unwrap();
        assert!(data2.deleted_task_uuids().is_empty());
        assert!(data2.deleted_tag_names().is_empty());
    }
}
