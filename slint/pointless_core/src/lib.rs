// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

pub mod calendar;
pub mod clock;
pub mod context;
pub mod data;
pub mod data_provider;
pub mod date_utils;
pub mod error;
pub mod tag;
pub mod task;
pub mod merger;
pub mod local_data;
pub mod utils;
pub mod supabase;
pub mod ical_parser;
pub mod caldav_client;
pub mod linux_calendar;

pub use calendar::{Calendar, CalendarEvent, CalendarProvider, DateRange};
pub use context::Context;
pub use data::{Data, DataPayload};
pub use data_provider::{DataProvider, DataProviderType};
pub use error::{PointlessError, Result};
pub use local_data::LocalData;
pub use merger::merge;
pub use supabase::SupabaseProvider;
pub use tag::Tag;
pub use task::Task;
