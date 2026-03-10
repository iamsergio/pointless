// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use std::io::{Read, Write};

use base64::engine::general_purpose::STANDARD;
use base64::Engine;
use flate2::read::GzDecoder;
use flate2::write::GzEncoder;
use flate2::Compression;
use reqwest::blocking::Client;
use reqwest::header::{AUTHORIZATION, CONTENT_TYPE};

use crate::data_provider::DataProvider;
use crate::error::PointlessError;
use crate::utils::{getenv_or_empty, trim};

pub struct SupabaseProvider {
    base_url: String,
    anon_key: String,
    access_token: String,
    refresh_token_value: String,
    user_id_value: String,
    default_user: String,
    default_password: String,
    client: Client,
}

impl SupabaseProvider {
    pub fn new(base_url: String, anon_key: String) -> Self {
        Self {
            base_url,
            anon_key,
            access_token: String::new(),
            refresh_token_value: String::new(),
            user_id_value: String::new(),
            default_user: String::new(),
            default_password: String::new(),
            client: Client::new(),
        }
    }

    pub fn create_default() -> Option<Self> {
        let url = getenv_or_empty("POINTLESS_SUPABASE_URL");
        let key = getenv_or_empty("POINTLESS_SUPABASE_ANON_KEY");
        if url.is_empty() || key.is_empty() {
            log::error!("POINTLESS_SUPABASE_URL or POINTLESS_SUPABASE_ANON_KEY not set");
            return None;
        }
        Some(Self::new(url, key))
    }

    fn retrieve_raw_data(&self) -> Result<String, PointlessError> {
        if self.access_token.is_empty() {
            return Err(PointlessError::NotAuthenticated);
        }

        let url = format!("https://{}/rest/v1/Documents?select=data", self.base_url);
        let response = self
            .client
            .get(&url)
            .header("apikey", &self.anon_key)
            .header(AUTHORIZATION, format!("Bearer {}", self.access_token))
            .send()?;

        let status = response.status();
        if status == reqwest::StatusCode::UNAUTHORIZED {
            return Err(PointlessError::NotAuthenticated);
        }

        if !status.is_success() {
            return Err(PointlessError::General(format!(
                "Failed to retrieve data: HTTP {}",
                status
            )));
        }

        let body = response.text()?;
        let json: serde_json::Value = serde_json::from_str(&body)?;

        let arr = json
            .as_array()
            .ok_or_else(|| PointlessError::General("Expected JSON array".into()))?;

        if arr.is_empty() {
            return Err(PointlessError::General("No documents found".into()));
        }

        let data = arr[0]["data"]
            .as_str()
            .ok_or_else(|| PointlessError::General("Missing 'data' field".into()))?;

        Ok(data.to_string())
    }

    fn compress(data: &str) -> Result<Vec<u8>, PointlessError> {
        let mut encoder = GzEncoder::new(Vec::new(), Compression::default());
        encoder
            .write_all(data.as_bytes())
            .map_err(|e| PointlessError::Compression(e.to_string()))?;
        encoder
            .finish()
            .map_err(|e| PointlessError::Compression(e.to_string()))
    }

    fn decompress(compressed_data: &[u8]) -> Result<String, PointlessError> {
        let mut decoder = GzDecoder::new(compressed_data);
        let mut result = String::new();
        decoder
            .read_to_string(&mut result)
            .map_err(|e| PointlessError::Compression(e.to_string()))?;
        Ok(result)
    }

    fn base64_decode(input: &str) -> Result<Vec<u8>, PointlessError> {
        STANDARD
            .decode(input)
            .map_err(|e| PointlessError::General(format!("Base64 decode error: {}", e)))
    }

    fn base64_encode(data: &[u8]) -> String {
        STANDARD.encode(data)
    }
}

impl DataProvider for SupabaseProvider {
    fn login(&mut self, email: &str, password: &str) -> bool {
        let url = format!(
            "https://{}/auth/v1/token?grant_type=password",
            self.base_url
        );

        let body = serde_json::json!({
            "email": email,
            "password": password,
        });

        let response = match self
            .client
            .post(&url)
            .header("apikey", &self.anon_key)
            .header(CONTENT_TYPE, "application/json")
            .json(&body)
            .send()
        {
            Ok(r) => r,
            Err(e) => {
                log::error!("Login request failed: {}", e);
                return false;
            }
        };

        if response.status() != reqwest::StatusCode::OK {
            log::error!("Login failed with status: {}", response.status());
            return false;
        }

        let body_text = match response.text() {
            Ok(t) => t,
            Err(e) => {
                log::error!("Failed to read login response: {}", e);
                return false;
            }
        };

        let json: serde_json::Value = match serde_json::from_str(&body_text) {
            Ok(v) => v,
            Err(e) => {
                log::error!("Failed to parse login response: {}", e);
                return false;
            }
        };

        let access = json["access_token"].as_str().unwrap_or_default();
        let user_id = json["user"]["id"].as_str().unwrap_or_default();
        let refresh = json["refresh_token"].as_str().unwrap_or_default();

        if access.is_empty() || user_id.is_empty() {
            log::error!("Login response missing required fields");
            return false;
        }

        self.access_token = access.to_string();
        self.user_id_value = user_id.to_string();
        self.refresh_token_value = refresh.to_string();

        if let Some(expires_in) = json["expires_in"].as_i64() {
            log::info!("Login successful, token expires in {} seconds", expires_in);
        } else {
            log::info!("Login successful");
        }

        true
    }

    fn login_with_defaults(&mut self) -> bool {
        let (username, password) = self.default_login_password();
        if username.is_empty() || password.is_empty() {
            log::error!("Default login credentials not available");
            return false;
        }
        self.login(&username, &password)
    }

    fn default_login_password(&self) -> (String, String) {
        let username = if !self.default_user.is_empty() {
            self.default_user.clone()
        } else {
            trim(&getenv_or_empty("POINTLESS_USERNAME")).to_string()
        };

        let password = if !self.default_password.is_empty() {
            self.default_password.clone()
        } else {
            trim(&getenv_or_empty("POINTLESS_PASSWORD")).to_string()
        };

        (username, password)
    }

    fn is_authenticated(&mut self) -> bool {
        if self.access_token.is_empty() {
            return false;
        }

        let url = format!("https://{}/auth/v1/user", self.base_url);
        let response = match self
            .client
            .get(&url)
            .header("apikey", &self.anon_key)
            .header(AUTHORIZATION, format!("Bearer {}", self.access_token))
            .send()
        {
            Ok(r) => r,
            Err(e) => {
                log::error!("Auth check failed: {}", e);
                return false;
            }
        };

        let status = response.status();
        if status == reqwest::StatusCode::OK {
            return true;
        }

        if status == reqwest::StatusCode::UNAUTHORIZED && !self.refresh_token_value.is_empty() {
            log::info!("Access token expired, attempting refresh");
            return self.refresh_access_token();
        }

        self.logout();
        false
    }

    fn logout(&mut self) {
        self.access_token.clear();
        self.refresh_token_value.clear();
        self.user_id_value.clear();
    }

    fn push_data(&mut self, data: &str) -> Result<(), PointlessError> {
        if self.access_token.is_empty() {
            return Err(PointlessError::NotAuthenticated);
        }

        let compressed = Self::compress(data)?;
        let encoded = Self::base64_encode(&compressed);

        let url = format!("https://{}/rest/v1/Documents", self.base_url);
        let body = serde_json::json!({
            "data": encoded,
            "id": 0,
        });

        let response = self
            .client
            .post(&url)
            .header("apikey", &self.anon_key)
            .header(AUTHORIZATION, format!("Bearer {}", self.access_token))
            .header(CONTENT_TYPE, "application/json")
            .header("Prefer", "return=minimal,resolution=merge-duplicates")
            .json(&body)
            .send()?;

        let status = response.status();

        if status == reqwest::StatusCode::UNAUTHORIZED {
            return Err(PointlessError::NotAuthenticated);
        }

        if status == reqwest::StatusCode::OK
            || status == reqwest::StatusCode::CREATED
            || status == reqwest::StatusCode::NO_CONTENT
        {
            log::info!("Data pushed successfully");
            return Ok(());
        }

        Err(PointlessError::General(format!(
            "Push failed with HTTP {}",
            status
        )))
    }

    fn pull_data(&mut self) -> Result<String, PointlessError> {
        let raw = self.retrieve_raw_data()?;
        let compressed = Self::base64_decode(&raw)?;
        Self::decompress(&compressed)
    }

    fn access_token(&self) -> &str {
        &self.access_token
    }

    fn refresh_token(&self) -> &str {
        &self.refresh_token_value
    }

    fn user_id(&self) -> &str {
        &self.user_id_value
    }

    fn set_access_token(&mut self, token: &str) {
        self.access_token = token.to_string();
    }

    fn set_refresh_token(&mut self, token: &str) {
        self.refresh_token_value = token.to_string();
    }

    fn set_user_id(&mut self, user_id: &str) {
        self.user_id_value = user_id.to_string();
    }

    fn refresh_access_token(&mut self) -> bool {
        let url = format!(
            "https://{}/auth/v1/token?grant_type=refresh_token",
            self.base_url
        );

        let body = serde_json::json!({
            "refresh_token": self.refresh_token_value,
        });

        let response = match self
            .client
            .post(&url)
            .header("apikey", &self.anon_key)
            .header(CONTENT_TYPE, "application/json")
            .json(&body)
            .send()
        {
            Ok(r) => r,
            Err(e) => {
                log::error!("Refresh token request failed: {}", e);
                return false;
            }
        };

        if response.status() != reqwest::StatusCode::OK {
            log::error!("Refresh token failed with status: {}", response.status());
            self.logout();
            return false;
        }

        let body_text = match response.text() {
            Ok(t) => t,
            Err(e) => {
                log::error!("Failed to read refresh response: {}", e);
                return false;
            }
        };

        let json: serde_json::Value = match serde_json::from_str(&body_text) {
            Ok(v) => v,
            Err(e) => {
                log::error!("Failed to parse refresh response: {}", e);
                return false;
            }
        };

        let access = json["access_token"].as_str().unwrap_or_default();
        let refresh = json["refresh_token"].as_str().unwrap_or_default();

        if access.is_empty() {
            log::error!("Refresh response missing access_token");
            self.logout();
            return false;
        }

        self.access_token = access.to_string();
        if !refresh.is_empty() {
            self.refresh_token_value = refresh.to_string();
        }

        log::info!("Access token refreshed successfully");
        true
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_login_logout() {
        let username = std::env::var("POINTLESS_DEBUG_USERNAME")
            .expect("POINTLESS_DEBUG_USERNAME env var not set");
        let password = std::env::var("POINTLESS_DEBUG_PASSWORD")
            .expect("POINTLESS_DEBUG_PASSWORD env var not set");

        let mut supabase =
            SupabaseProvider::create_default().expect("Failed to create SupabaseProvider");
        assert!(supabase.login(&username, &password));
        supabase.logout();
    }
}
