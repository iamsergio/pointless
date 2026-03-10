// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use std::path::{Path, PathBuf};
use std::sync::Mutex;

use crate::data_provider::DataProviderType;

static CONTEXT: Mutex<Option<Context>> = Mutex::new(None);
static CLIENT_DATA_DIR: Mutex<Option<String>> = Mutex::new(None);

const STARTUP_OPTION_RESTORE_AUTH: u32 = 1;

#[derive(Clone, Debug)]
pub struct Context {
    pub provider_type: DataProviderType,
    pub local_file_path: String,
    pub startup_options: u32,
    pub read_only: bool,
}

impl Context {
    pub fn new(
        provider_type: DataProviderType,
        local_file_path: String,
        startup_options: u32,
        read_only: bool,
    ) -> Self {
        if !local_file_path.is_empty() {
            let path = Path::new(&local_file_path);
            if let Some(parent) = path.parent() {
                if !parent.as_os_str().is_empty() {
                    let _ = std::fs::create_dir_all(parent);
                }
            }
        }

        Self {
            provider_type,
            local_file_path,
            startup_options,
            read_only,
        }
    }

    pub fn should_restore_auth(&self) -> bool {
        (self.startup_options & STARTUP_OPTION_RESTORE_AUTH) != 0
    }

    pub fn set_context(context: Context) {
        let mut guard = CONTEXT.lock().expect("CONTEXT mutex poisoned");
        *guard = Some(context);
    }

    pub fn get() -> Context {
        let guard = CONTEXT.lock().expect("CONTEXT mutex poisoned");
        guard.clone().expect("FATAL: Context not set")
    }

    pub fn has_context() -> bool {
        let guard = CONTEXT.lock().expect("CONTEXT mutex poisoned");
        guard.is_some()
    }

    pub fn client_data_dir() -> String {
        let guard = CLIENT_DATA_DIR.lock().expect("CLIENT_DATA_DIR mutex poisoned");
        if let Some(ref dir) = *guard {
            return dir.clone();
        }
        drop(guard);

        std::env::var("POINTLESS_CLIENT_DATA_DIR")
            .expect("FATAL: POINTLESS_CLIENT_DATA_DIR environment variable is not set")
    }

    pub fn set_client_data_dir(dir: &str) {
        let mut guard = CLIENT_DATA_DIR.lock().expect("CLIENT_DATA_DIR mutex poisoned");
        *guard = Some(dir.to_string());
    }

    pub fn default_context_for_supabase_testing(startup_options: u32) -> Context {
        let path: PathBuf = [&Self::client_data_dir(), "debug-pointless.json"]
            .iter()
            .collect();
        Context::new(
            DataProviderType::TestSupabase,
            path.to_string_lossy().into_owned(),
            startup_options,
            false,
        )
    }

    pub fn default_context_for_supabase_release(startup_options: u32) -> Context {
        let path: PathBuf = [&Self::client_data_dir(), "pointless.json"]
            .iter()
            .collect();
        Context::new(
            DataProviderType::Supabase,
            path.to_string_lossy().into_owned(),
            startup_options,
            false,
        )
    }
}
