// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use crate::caldav_client::{CalDavClient, CalDavConfig};
use crate::calendar::{Calendar, CalendarEvent, CalendarProvider, DateRange};
use crate::utils::getenv_or_empty;

pub struct LinuxCalendarProvider {
    client: Option<CalDavClient>,
    home_set_url: String,
}

impl LinuxCalendarProvider {
    pub fn new(caldav_url: &str, caldav_username: &str, caldav_password: &str) -> Self {
        let url = if caldav_url.is_empty() {
            getenv_or_empty("POINTLESS_CALDAV_URL")
        } else {
            caldav_url.to_string()
        };

        let username = if caldav_username.is_empty() {
            getenv_or_empty("POINTLESS_CALDAV_USERNAME")
        } else {
            caldav_username.to_string()
        };

        let password = if caldav_password.is_empty() {
            getenv_or_empty("POINTLESS_CALDAV_PASSWORD")
        } else {
            caldav_password.to_string()
        };

        if url.is_empty() {
            log::info!("POINTLESS_CALDAV_URL not set, CalDAV integration disabled");
            return Self {
                client: None,
                home_set_url: String::new(),
            };
        }

        let config = CalDavConfig {
            server_url: url,
            username,
            password,
        };

        let client = CalDavClient::new(config);
        match client.discover_calendar_home_set() {
            Ok(home_set_url) => {
                log::info!("CalDAV calendar home set: {}", home_set_url);
                Self {
                    client: Some(client),
                    home_set_url,
                }
            }
            Err(e) => {
                log::warn!("{}", e);
                Self {
                    client: None,
                    home_set_url: String::new(),
                }
            }
        }
    }
}

impl CalendarProvider for LinuxCalendarProvider {
    fn is_configured(&self) -> bool {
        self.client.is_some()
    }

    fn get_calendars(&self) -> Vec<Calendar> {
        let Some(client) = &self.client else {
            return Vec::new();
        };
        client.fetch_calendars(&self.home_set_url)
    }

    fn get_events(&self, range: &DateRange, calendar_ids: &[String]) -> Vec<CalendarEvent> {
        let Some(client) = &self.client else {
            return Vec::new();
        };

        let calendars = client.fetch_calendars(&self.home_set_url);
        let mut all_events = Vec::new();

        for cal in &calendars {
            let requested = calendar_ids.is_empty()
                || calendar_ids.iter().any(|id| id == &cal.id);

            if !requested {
                continue;
            }

            let calendar_url = CalDavClient::resolve_url(&self.home_set_url, &cal.id);
            let events = client.fetch_events(&calendar_url, &cal.id, &cal.title, range);
            all_events.extend(events);
        }

        all_events
    }
}

pub fn create_calendar_provider(
    url: &str,
    username: &str,
    password: &str,
) -> Box<dyn CalendarProvider> {
    Box::new(LinuxCalendarProvider::new(url, username, password))
}
