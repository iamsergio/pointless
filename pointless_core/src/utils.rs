// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pub fn trim(s: &str) -> &str {
    s.trim()
}

pub fn getenv_or_empty(name: &str) -> String {
    std::env::var(name).unwrap_or_default()
}

pub fn is_ios() -> bool {
    cfg!(target_os = "ios")
}
