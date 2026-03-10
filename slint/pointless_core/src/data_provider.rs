// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use std::fmt;

use crate::error::PointlessError;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DataProviderType {
    None,
    TestsLocal,
    Supabase,
    TestSupabase,
}

impl fmt::Display for DataProviderType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            DataProviderType::None => write!(f, "None"),
            DataProviderType::TestsLocal => write!(f, "TestsLocal"),
            DataProviderType::Supabase => write!(f, "Supabase"),
            DataProviderType::TestSupabase => write!(f, "TestSupabase"),
        }
    }
}

pub trait DataProvider {
    fn is_authenticated(&mut self) -> bool;
    fn login(&mut self, email: &str, password: &str) -> bool;
    fn login_with_defaults(&mut self) -> bool;
    fn default_login_password(&self) -> (String, String);
    fn logout(&mut self);
    fn pull_data(&mut self) -> Result<String, PointlessError>;
    fn push_data(&mut self, data: &str) -> Result<(), PointlessError>;
    fn access_token(&self) -> &str;
    fn refresh_token(&self) -> &str;
    fn user_id(&self) -> &str;
    fn set_access_token(&mut self, token: &str);
    fn set_refresh_token(&mut self, token: &str);
    fn set_user_id(&mut self, user_id: &str);
    fn refresh_access_token(&mut self) -> bool;
}
