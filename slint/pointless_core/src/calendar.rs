// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use chrono::{DateTime, Utc};

#[derive(Debug, Clone, Default)]
pub struct Calendar {
    pub id: String,
    pub title: String,
    pub color: String,
    pub writeable: bool,
}

#[derive(Debug, Clone, Copy)]
pub struct DateRange {
    pub start: DateTime<Utc>,
    pub end: DateTime<Utc>,
}

#[derive(Debug, Clone)]
pub struct CalendarEvent {
    pub event_id: String,
    pub calendar_id: String,
    pub calendar_name: String,
    pub title: String,
    pub start_date: DateTime<Utc>,
    pub end_date: DateTime<Utc>,
    pub is_all_day: bool,
}

pub trait CalendarProvider {
    fn is_configured(&self) -> bool;
    fn get_calendars(&self) -> Vec<Calendar>;
    fn get_events(&self, range: &DateRange, calendar_ids: &[String]) -> Vec<CalendarEvent>;
}
