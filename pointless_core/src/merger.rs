// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use crate::data::DataPayload;

pub fn merge(local_data: &DataPayload, server_data: &DataPayload) -> DataPayload {
    if server_data.tasks.is_empty() && server_data.tags.is_empty() && server_data.revision == 0 {
        let mut result = local_data.clone();
        result.revision = 0;
        for task in &mut result.tasks {
            task.needs_sync_to_server = false;
        }
        for tag in &mut result.tags {
            tag.needs_sync_to_server = false;
        }
        return result;
    }

    if local_data.revision == -1 && local_data.tasks.is_empty() && local_data.tags.is_empty() {
        return server_data.clone();
    }

    if local_data.revision > server_data.revision {
        log::warn!(
            "Client revision {} > Server revision {}",
            local_data.revision,
            server_data.revision
        );
        return server_data.clone();
    }

    let mut current = server_data.clone();
    let mut changed = false;

    for tag in &local_data.tags {
        let exists = current.tags.iter().any(|t| t.name == tag.name);
        if !exists {
            let mut new_tag = tag.clone();
            new_tag.revision = 0;
            new_tag.needs_sync_to_server = false;
            current.tags.push(new_tag);
            changed = true;
        }
    }

    for incoming_task in &local_data.tasks {
        if !incoming_task.needs_sync_to_server {
            continue;
        }

        let found = current
            .tasks
            .iter()
            .position(|t| t.uuid == incoming_task.uuid);

        match found {
            None => {
                if incoming_task.revision <= 0 {
                    let mut new_task = incoming_task.clone();
                    new_task.revision = 0;
                    new_task.needs_sync_to_server = false;
                    current.tasks.push(new_task);
                    changed = true;
                }
            }
            Some(idx) => {
                if current.tasks[idx].revision == incoming_task.revision {
                    current.tasks[idx] = incoming_task.clone();
                    current.tasks[idx].revision += 1;
                    current.tasks[idx].needs_sync_to_server = false;
                    changed = true;
                } else if current.tasks[idx].revision > incoming_task.revision {
                    current.tasks[idx].merge_conflict(incoming_task);
                    current.tasks[idx].revision += 1;
                    current.tasks[idx].needs_sync_to_server = false;
                    changed = true;
                } else {
                    log::warn!(
                        "Task {} has greater revision on client ({}) than server ({})",
                        incoming_task.title,
                        incoming_task.revision,
                        current.tasks[idx].revision
                    );
                }
            }
        }
    }

    for uuid in &local_data.deleted_task_uuids {
        if let Some(pos) = current.tasks.iter().position(|t| t.uuid == *uuid) {
            current.tasks.remove(pos);
            changed = true;
        }
    }

    for tag_name in &local_data.deleted_tag_names {
        if let Some(pos) = current.tags.iter().position(|t| t.name == *tag_name) {
            current.tags.remove(pos);
            changed = true;
        }
    }

    if changed {
        current.revision += 1;
        for t in &mut current.tasks {
            t.needs_sync_to_server = false;
        }
        for t in &mut current.tags {
            t.needs_sync_to_server = false;
        }
    }

    current
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::tag::Tag;
    use crate::task::Task;
    use chrono::{TimeZone, Utc};

    fn make_task(uuid: &str, title: &str) -> Task {
        Task::new(
            uuid.to_string(),
            Utc.with_ymd_and_hms(2025, 6, 15, 12, 0, 0).unwrap(),
            title.to_string(),
        )
    }

    fn make_tag(name: &str) -> Tag {
        Tag {
            name: name.to_string(),
            ..Default::default()
        }
    }

    #[test]
    fn test_empty_server_init() {
        let mut local = DataPayload::default();
        local.revision = 5;
        let mut t = make_task("u1", "Task 1");
        t.needs_sync_to_server = true;
        local.tasks.push(t);
        let mut tag = make_tag("work");
        tag.needs_sync_to_server = true;
        local.tags.push(tag);

        let server = DataPayload {
            revision: 0,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };

        let result = merge(&local, &server);
        assert_eq!(result.revision, 0);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tags.len(), 1);
        assert!(!result.tasks[0].needs_sync_to_server);
        assert!(!result.tags[0].needs_sync_to_server);
    }

    #[test]
    fn test_empty_client_adoption() {
        let local = DataPayload::default();
        assert_eq!(local.revision, -1);

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        server.tasks.push(make_task("s1", "Server Task"));
        server.tags.push(make_tag("server_tag"));

        let result = merge(&local, &server);
        assert_eq!(result.revision, 3);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tasks[0].title, "Server Task");
        assert_eq!(result.tags.len(), 1);
    }

    #[test]
    fn test_client_ahead_of_server() {
        let mut local = DataPayload {
            revision: 10,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        local.tasks.push(make_task("u1", "Local"));

        let mut server = DataPayload {
            revision: 5,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        server.tasks.push(make_task("s1", "Server"));

        let result = merge(&local, &server);
        assert_eq!(result.revision, 5);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tasks[0].title, "Server");
    }

    #[test]
    fn test_fast_forward_new_task() {
        let mut local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut new_task = make_task("u1", "New Local Task");
        new_task.revision = 0;
        new_task.needs_sync_to_server = true;
        local.tasks.push(new_task);

        let server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };

        let result = merge(&local, &server);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tasks[0].title, "New Local Task");
        assert_eq!(result.tasks[0].revision, 0);
        assert!(!result.tasks[0].needs_sync_to_server);
        assert_eq!(result.revision, 4);
    }

    #[test]
    fn test_fast_forward_same_revision() {
        let mut local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut local_task = make_task("u1", "Updated Title");
        local_task.revision = 2;
        local_task.needs_sync_to_server = true;
        local.tasks.push(local_task);

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut server_task = make_task("u1", "Old Title");
        server_task.revision = 2;
        server.tasks.push(server_task);

        let result = merge(&local, &server);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tasks[0].title, "Updated Title");
        assert_eq!(result.tasks[0].revision, 3);
        assert!(!result.tasks[0].needs_sync_to_server);
        assert_eq!(result.revision, 4);
    }

    #[test]
    fn test_conflict_merge() {
        let mut local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut local_task = make_task("u1", "Local Title");
        local_task.revision = 1;
        local_task.needs_sync_to_server = true;
        local_task.is_important = true;
        local.tasks.push(local_task);

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut server_task = make_task("u1", "Server Title");
        server_task.revision = 5;
        server_task.is_done = true;
        server.tasks.push(server_task);

        let result = merge(&local, &server);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tasks[0].revision, 6);
        assert!(result.tasks[0].is_important);
        assert!(!result.tasks[0].is_done);
        assert!(!result.tasks[0].needs_sync_to_server);
        assert_eq!(result.revision, 4);
    }

    #[test]
    fn test_deleted_tasks() {
        let local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec!["u1".to_string()],
            deleted_tag_names: vec![],
        };

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        server.tasks.push(make_task("u1", "To Delete"));
        server.tasks.push(make_task("u2", "Keep"));

        let result = merge(&local, &server);
        assert_eq!(result.tasks.len(), 1);
        assert_eq!(result.tasks[0].uuid, "u2");
        assert_eq!(result.revision, 4);
    }

    #[test]
    fn test_deleted_tags() {
        let local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec!["remove_me".to_string()],
        };

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        server.tags.push(make_tag("remove_me"));
        server.tags.push(make_tag("keep_me"));

        let result = merge(&local, &server);
        assert_eq!(result.tags.len(), 1);
        assert_eq!(result.tags[0].name, "keep_me");
        assert_eq!(result.revision, 4);
    }

    #[test]
    fn test_add_new_tags() {
        let mut local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        local.tags.push(make_tag("new_tag"));

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        server.tags.push(make_tag("existing_tag"));

        let result = merge(&local, &server);
        assert_eq!(result.tags.len(), 2);
        let new_tag = result.tags.iter().find(|t| t.name == "new_tag").unwrap();
        assert_eq!(new_tag.revision, 0);
        assert!(!new_tag.needs_sync_to_server);
        assert_eq!(result.revision, 4);
    }

    #[test]
    fn test_no_change_no_revision_increment() {
        let local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        server.tasks.push(make_task("u1", "Unchanged"));

        let result = merge(&local, &server);
        assert_eq!(result.revision, 3);
        assert_eq!(result.tasks.len(), 1);
    }

    #[test]
    fn test_task_not_needing_sync_is_skipped() {
        let mut local = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut task = make_task("u1", "No Sync");
        task.needs_sync_to_server = false;
        task.revision = 2;
        local.tasks.push(task);

        let mut server = DataPayload {
            revision: 3,
            tasks: vec![],
            tags: vec![],
            deleted_task_uuids: vec![],
            deleted_tag_names: vec![],
        };
        let mut server_task = make_task("u1", "Server Version");
        server_task.revision = 2;
        server.tasks.push(server_task);

        let result = merge(&local, &server);
        assert_eq!(result.tasks[0].title, "Server Version");
        assert_eq!(result.revision, 3);
    }
}
