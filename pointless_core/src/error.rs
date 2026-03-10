// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#[derive(Debug, thiserror::Error)]
pub enum PointlessError {
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),
    #[error("JSON error: {0}")]
    Json(#[from] serde_json::Error),
    #[error("HTTP error: {0}")]
    Http(#[from] reqwest::Error),
    #[error("{0}")]
    General(String),
    #[error("Not authenticated")]
    NotAuthenticated,
    #[error("Compression error: {0}")]
    Compression(String),
}

pub type Result<T> = std::result::Result<T, PointlessError>;
