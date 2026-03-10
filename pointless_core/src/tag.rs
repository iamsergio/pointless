// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use serde::{Deserialize, Serialize};

pub const BUILTIN_TAG_SOON: &str = "soon";
pub const BUILTIN_TAG_CURRENT: &str = "current";
pub const BUILTIN_TAG_EVENING: &str = "evening";

const BUILTIN_TAGS: [&str; 3] = [BUILTIN_TAG_SOON, BUILTIN_TAG_CURRENT, BUILTIN_TAG_EVENING];

pub fn tag_is_builtin(name: &str) -> bool {
    BUILTIN_TAGS.contains(&name)
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Tag {
    #[serde(default = "default_revision")]
    pub revision: i32,

    #[serde(skip)]
    pub needs_sync_to_server: bool,

    pub name: String,
}

fn default_revision() -> i32 {
    -1
}

impl Default for Tag {
    fn default() -> Self {
        Self {
            revision: -1,
            needs_sync_to_server: false,
            name: String::new(),
        }
    }
}

impl PartialEq for Tag {
    fn eq(&self, other: &Self) -> bool {
        self.name == other.name
    }
}

impl Tag {
    pub fn is_builtin(&self) -> bool {
        tag_is_builtin(&self.name)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_builtin_tags() {
        assert!(tag_is_builtin("soon"));
        assert!(tag_is_builtin("current"));
        assert!(tag_is_builtin("evening"));
        assert!(!tag_is_builtin("custom"));
    }

    #[test]
    fn test_tag_is_builtin_method() {
        let tag = Tag {
            name: "soon".to_string(),
            ..Default::default()
        };
        assert!(tag.is_builtin());

        let tag = Tag {
            name: "custom".to_string(),
            ..Default::default()
        };
        assert!(!tag.is_builtin());
    }

    #[test]
    fn test_tag_equality_by_name_only() {
        let a = Tag {
            revision: 1,
            needs_sync_to_server: true,
            name: "foo".to_string(),
        };
        let b = Tag {
            revision: 99,
            needs_sync_to_server: false,
            name: "foo".to_string(),
        };
        assert_eq!(a, b);
    }

    #[test]
    fn test_serde_skips_needs_sync() {
        let tag = Tag {
            revision: 5,
            needs_sync_to_server: true,
            name: "test".to_string(),
        };
        let json = serde_json::to_string(&tag).unwrap();
        assert!(!json.contains("needs_sync_to_server"));

        let deserialized: Tag = serde_json::from_str(&json).unwrap();
        assert!(!deserialized.needs_sync_to_server);
        assert_eq!(deserialized.revision, 5);
        assert_eq!(deserialized.name, "test");
    }
}
