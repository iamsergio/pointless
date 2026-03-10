// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use std::fs;
use std::path::Path;

use log::info;

use crate::clock;
use crate::context::Context;
use crate::data::Data;
use crate::error::PointlessError;
use crate::tag::Tag;
use crate::task::Task;

pub struct LocalData {
    data: Data,
}

impl Default for LocalData {
    fn default() -> Self {
        Self::new()
    }
}

impl LocalData {
    pub fn new() -> Self {
        Self {
            data: Data::new(),
        }
    }

    pub fn load_data_from_file(&mut self) -> Result<(), PointlessError> {
        let path = self.get_data_file_path();
        match self.load_data_from_path(&path) {
            Ok(data) => {
                self.data = data;
                Ok(())
            }
            Err(e) => {
                self.data = Data::new();
                Err(e)
            }
        }
    }

    pub fn load_data_from_path(&self, filename: &str) -> Result<Data, PointlessError> {
        let path = Path::new(filename);
        if !path.exists() {
            return Ok(Data::new());
        }

        let json_content = fs::read_to_string(path)?;

        if json_content.is_empty() {
            return Err(PointlessError::General(format!(
                "Failed to read JSON from file: {}",
                filename
            )));
        }

        Data::from_json(&json_content)
    }

    pub fn save(&self) -> Result<(), PointlessError> {
        let ctx = Context::get();
        if ctx.read_only {
            info!("Skipping save, read-only");
            return Ok(());
        }

        info!(
            "Saving, numTasks={}, revision={}",
            self.data.task_count(),
            self.data.revision()
        );

        let filename = self.get_data_file_path();
        let json = self.data.to_json()?;
        fs::write(&filename, json)?;
        self.data.needs_local_save.set(false);
        Ok(())
    }

    pub fn data(&self) -> &Data {
        &self.data
    }

    pub fn data_mut(&mut self) -> &mut Data {
        &mut self.data
    }

    pub fn set_data(&mut self, data: Data) {
        self.data = data;
    }

    pub fn set_data_and_save(&mut self, data: Data) -> Result<(), PointlessError> {
        self.set_data(data);
        self.save()
    }

    // --- Delegating accessors ---

    pub fn task_count(&self) -> usize {
        self.data.task_count()
    }

    pub fn task_at(&self, index: usize) -> &Task {
        self.data.task_at(index)
    }

    pub fn task_for_uuid(&self, uuid: &str) -> Option<&Task> {
        self.data.task_for_uuid(uuid)
    }

    pub fn task_for_uuid_mut(&mut self, uuid: &str) -> Option<&mut Task> {
        self.data.task_for_uuid_mut(uuid)
    }

    pub fn task_for_title(&self, title: &str) -> Option<&Task> {
        self.data.task_for_title(title)
    }

    pub fn tag_count(&self) -> usize {
        self.data.tag_count()
    }

    pub fn tag_at(&self, index: usize) -> &Tag {
        self.data.tag_at(index)
    }

    pub fn deleted_tags(&self) -> &[String] {
        self.data.deleted_tag_names()
    }

    pub fn deleted_tasks(&self) -> &[String] {
        self.data.deleted_task_uuids()
    }

    // --- Task mutation ---

    pub fn add_task(&mut self, mut task: Task) -> bool {
        if task.uuid.is_empty() {
            log::error!("add_task: task has empty uuid");
            return false;
        }

        let now = clock::now();
        task.creation_timestamp = now;
        task.modification_timestamp = Some(now);
        task.needs_sync_to_server = true;
        self.data.needs_local_save.set(true);
        self.data.add_task(task);
        true
    }

    pub fn update_task(&mut self, mut task: Task) -> bool {
        task.modification_timestamp = Some(clock::now());
        task.needs_sync_to_server = true;
        self.data.needs_local_save.set(true);
        self.data.set_task(task)
    }

    pub fn remove_task(&mut self, uuid: &str) -> bool {
        if self.data.remove_task(uuid) {
            self.data.add_deleted_task_uuid(uuid.to_string());
            true
        } else {
            false
        }
    }

    pub fn remove_tag(&mut self, tag_name: &str) -> bool {
        if self.data.remove_tag(tag_name) {
            self.data.add_deleted_tag_name(tag_name.to_string());
            true
        } else {
            false
        }
    }

    pub fn cleanup_old_data(&mut self) -> usize {
        let uuids_to_remove: Vec<String> = self
            .data
            .payload
            .tasks
            .iter()
            .filter(|t| t.should_be_cleaned_up())
            .map(|t| t.uuid.clone())
            .collect();

        let count = uuids_to_remove.len();
        for uuid in uuids_to_remove {
            self.remove_task(&uuid);
        }

        if count > 0 {
            self.data.needs_local_save.set(true);
        }

        count
    }

    pub fn delete_calendar_tasks(&mut self) -> usize {
        let uuids_to_remove: Vec<String> = self
            .data
            .payload
            .tasks
            .iter()
            .filter(|t| t.uuid_in_device_calendar.is_some())
            .map(|t| t.uuid.clone())
            .collect();

        let count = uuids_to_remove.len();
        for uuid in uuids_to_remove {
            self.remove_task(&uuid);
        }

        count
    }

    pub fn deduplicate_calendar_tasks(&mut self) -> usize {
        let duplicate_uuids = self.data.find_duplicate_calendar_task_uuids();
        let count = duplicate_uuids.len();
        for uuid in duplicate_uuids {
            self.remove_task(&uuid);
        }

        count
    }

    pub fn clear_server_sync_bits(&mut self) {
        self.data.clear_server_sync_bits();
    }

    // --- Private ---

    fn get_data_file_path(&self) -> String {
        Context::get().local_file_path.clone()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::context::Context;
    use crate::data_provider::DataProviderType;
    use chrono::{TimeZone, Utc};
    use std::fs;
    use std::sync::{LazyLock, Mutex, MutexGuard};

    static CONTEXT_MUTEX: LazyLock<Mutex<()>> = LazyLock::new(|| Mutex::new(()));

    fn setup_context(path: &str) -> MutexGuard<'static, ()> {
        let guard = CONTEXT_MUTEX.lock().unwrap();
        Context::set_context(Context::new(
            DataProviderType::TestsLocal,
            path.to_string(),
            0,
            false,
        ));
        guard
    }

    fn setup_readonly_context(path: &str) -> MutexGuard<'static, ()> {
        let guard = CONTEXT_MUTEX.lock().unwrap();
        Context::set_context(Context::new(
            DataProviderType::TestsLocal,
            path.to_string(),
            0,
            true,
        ));
        guard
    }

    fn make_task(uuid: &str, title: &str) -> Task {
        Task::new(
            uuid.to_string(),
            Utc.with_ymd_and_hms(2025, 6, 15, 12, 0, 0).unwrap(),
            title.to_string(),
        )
    }

    #[test]
    fn test_new_local_data_is_empty() {
        let ld = LocalData::new();
        assert_eq!(ld.task_count(), 0);
        assert_eq!(ld.tag_count(), 0);
    }

    #[test]
    fn test_add_task() {
        let mut ld = LocalData::new();
        let task = make_task("u1", "Task 1");
        assert!(ld.add_task(task));
        assert_eq!(ld.task_count(), 1);
        assert!(ld.task_for_uuid("u1").is_some());
    }

    #[test]
    fn test_add_task_empty_uuid_fails() {
        let mut ld = LocalData::new();
        let task = make_task("", "No UUID");
        assert!(!ld.add_task(task));
        assert_eq!(ld.task_count(), 0);
    }

    #[test]
    fn test_add_task_sets_timestamps() {
        clock::set_test_now(
            Utc.with_ymd_and_hms(2025, 7, 1, 10, 0, 0).unwrap(),
        );
        let mut ld = LocalData::new();
        let task = make_task("u1", "Task 1");
        ld.add_task(task);
        let t = ld.task_for_uuid("u1").unwrap();
        assert_eq!(
            t.creation_timestamp,
            Utc.with_ymd_and_hms(2025, 7, 1, 10, 0, 0).unwrap()
        );
        assert_eq!(
            t.modification_timestamp,
            Some(Utc.with_ymd_and_hms(2025, 7, 1, 10, 0, 0).unwrap())
        );
        assert!(t.needs_sync_to_server);
        clock::reset();
    }

    #[test]
    fn test_update_task() {
        clock::set_test_now(
            Utc.with_ymd_and_hms(2025, 7, 1, 10, 0, 0).unwrap(),
        );
        let mut ld = LocalData::new();
        ld.add_task(make_task("u1", "Original"));

        clock::set_test_now(
            Utc.with_ymd_and_hms(2025, 7, 2, 10, 0, 0).unwrap(),
        );
        let mut updated = make_task("u1", "Updated");
        updated.is_important = true;
        assert!(ld.update_task(updated));

        let t = ld.task_for_uuid("u1").unwrap();
        assert_eq!(t.title, "Updated");
        assert!(t.is_important);
        assert_eq!(
            t.modification_timestamp,
            Some(Utc.with_ymd_and_hms(2025, 7, 2, 10, 0, 0).unwrap())
        );
        clock::reset();
    }

    #[test]
    fn test_update_nonexistent_task_returns_false() {
        let mut ld = LocalData::new();
        assert!(!ld.update_task(make_task("nope", "X")));
    }

    #[test]
    fn test_remove_task() {
        let mut ld = LocalData::new();
        ld.add_task(make_task("u1", "Task 1"));
        assert!(ld.remove_task("u1"));
        assert_eq!(ld.task_count(), 0);
        assert_eq!(ld.deleted_tasks().len(), 1);
        assert_eq!(ld.deleted_tasks()[0], "u1");
    }

    #[test]
    fn test_remove_nonexistent_task() {
        let mut ld = LocalData::new();
        assert!(!ld.remove_task("nope"));
        assert!(ld.deleted_tasks().is_empty());
    }

    #[test]
    fn test_remove_tag() {
        let mut ld = LocalData::new();
        ld.data_mut().add_tag(Tag {
            name: "work".to_string(),
            ..Default::default()
        });
        assert!(ld.remove_tag("work"));
        assert_eq!(ld.tag_count(), 0);
        assert_eq!(ld.deleted_tags().len(), 1);
        assert_eq!(ld.deleted_tags()[0], "work");
    }

    #[test]
    fn test_save_and_load_roundtrip() {
        let dir = std::env::temp_dir().join("pointless_test_local_data");
        let _ = fs::create_dir_all(&dir);
        let path = dir.join("test_roundtrip.json");
        let path_str = path.to_string_lossy().to_string();

        let _guard = setup_context(&path_str);

        let mut ld = LocalData::new();
        ld.add_task(make_task("u1", "Task 1"));
        ld.data_mut().add_tag(Tag {
            name: "work".to_string(),
            ..Default::default()
        });
        ld.save().unwrap();

        let ld2 = LocalData::new();
        let loaded = ld2.load_data_from_path(&path_str).unwrap();
        assert_eq!(loaded.task_count(), 1);
        assert_eq!(loaded.tag_count(), 1);
        assert_eq!(loaded.task_for_uuid("u1").unwrap().title, "Task 1");

        let _ = fs::remove_file(&path);
    }

    #[test]
    fn test_load_nonexistent_file_returns_empty_data() {
        let ld = LocalData::new();
        let data = ld.load_data_from_path("/tmp/nonexistent_pointless_file.json").unwrap();
        assert!(data.is_empty());
    }

    #[test]
    fn test_save_readonly_skips() {
        let dir = std::env::temp_dir().join("pointless_test_local_data");
        let _ = fs::create_dir_all(&dir);
        let path = dir.join("test_readonly.json");
        let path_str = path.to_string_lossy().to_string();
        let _ = fs::remove_file(&path);

        let _guard = setup_readonly_context(&path_str);

        let mut ld = LocalData::new();
        ld.add_task(make_task("u1", "Task 1"));
        ld.save().unwrap();

        assert!(!path.exists());
    }

    #[test]
    fn test_set_data_and_save() {
        let dir = std::env::temp_dir().join("pointless_test_local_data");
        let _ = fs::create_dir_all(&dir);
        let path = dir.join("test_set_data_and_save.json");
        let path_str = path.to_string_lossy().to_string();

        let _guard = setup_context(&path_str);

        let mut ld = LocalData::new();
        let mut data = Data::new();
        data.add_task(make_task("u1", "From set_data"));
        ld.set_data_and_save(data).unwrap();

        let ld2 = LocalData::new();
        let loaded = ld2.load_data_from_path(&path_str).unwrap();
        assert_eq!(loaded.task_count(), 1);

        let _ = fs::remove_file(&path);
    }

    #[test]
    fn test_clear_server_sync_bits() {
        let mut ld = LocalData::new();
        ld.add_task(make_task("u1", "Task 1"));
        ld.clear_server_sync_bits();
        let t = ld.task_for_uuid("u1").unwrap();
        assert!(!t.needs_sync_to_server);
        assert!(!ld.data().needs_upload.get());
        assert!(!ld.data().needs_local_save.get());
    }

    #[test]
    fn test_load_from_env_var() {
        let dir = std::env::temp_dir().join("pointless_test_env");
        let _ = fs::create_dir_all(&dir);
        let dest = dir.join("pointless.json");

        let source = std::path::Path::new(env!("CARGO_MANIFEST_DIR"))
            .parent()
            .unwrap()
            .join("src/core/tests/test.json");
        fs::copy(&source, &dest).expect("Failed to copy test.json");

        let path_str = dest.to_string_lossy().to_string();
        let _guard = setup_context(&path_str);

        let mut ld = LocalData::new();
        ld.load_data_from_file().unwrap();

        assert_eq!(ld.data().tag_count(), 3);
        assert!(ld.data().task_count() > 0);

        let _ = fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_delegating_accessors() {
        let mut ld = LocalData::new();
        ld.add_task(make_task("u1", "First"));
        ld.add_task(make_task("u2", "Second"));
        ld.data_mut().add_tag(Tag {
            name: "tag1".to_string(),
            ..Default::default()
        });

        assert_eq!(ld.task_count(), 2);
        assert_eq!(ld.task_at(0).uuid, "u1");
        assert!(ld.task_for_title("First").is_some());
        assert!(ld.task_for_title("Missing").is_none());
        assert_eq!(ld.tag_count(), 1);
        assert_eq!(ld.tag_at(0).name, "tag1");
    }
}
