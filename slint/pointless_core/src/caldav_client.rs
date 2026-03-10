// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

use crate::calendar::{Calendar, CalendarEvent, DateRange};
use crate::error::PointlessError;
use crate::ical_parser::parse_ical_events;
use reqwest::blocking::Client;
use reqwest::Method;

#[derive(Debug, Clone)]
pub struct CalDavConfig {
    pub server_url: String,
    pub username: String,
    pub password: String,
}

pub struct CalDavClient {
    config: CalDavConfig,
    client: Client,
}

fn ensure_trailing_slash(url: &mut String) {
    if !url.is_empty() && !url.ends_with('/') {
        url.push('/');
    }
}

fn local_name(name: &str) -> &str {
    match name.find(':') {
        Some(pos) => &name[pos + 1..],
        None => name,
    }
}

fn find_child_by_local_name<'a>(
    node: &'a roxmltree::Node<'a, 'a>,
    name: &str,
) -> Option<roxmltree::Node<'a, 'a>> {
    node.children()
        .find(|child| child.is_element() && local_name(child.tag_name().name()) == name)
}

fn find_descendant_by_local_name<'a>(
    node: &'a roxmltree::Node<'a, 'a>,
    name: &str,
) -> Option<roxmltree::Node<'a, 'a>> {
    let mut stack = vec![*node];
    while let Some(current) = stack.pop() {
        if current.is_element() && local_name(current.tag_name().name()) == name {
            return Some(current);
        }
        for child in current.children() {
            stack.push(child);
        }
    }
    None
}

fn format_time_range(range: &DateRange) -> String {
    let start = range.start.format("%Y%m%dT%H%M%SZ");
    let end = range.end.format("%Y%m%dT%H%M%SZ");
    format!("start=\"{start}\" end=\"{end}\"")
}

fn normalize_color(color: &str) -> String {
    if color.len() == 9 && color.starts_with('#') {
        color[..7].to_string()
    } else {
        color.to_string()
    }
}

fn get_text_content(node: &roxmltree::Node) -> String {
    node.children()
        .filter(|c| c.is_text())
        .map(|c| c.text().unwrap_or(""))
        .collect()
}

impl CalDavClient {
    pub fn new(mut config: CalDavConfig) -> Self {
        ensure_trailing_slash(&mut config.server_url);
        Self {
            config,
            client: Client::new(),
        }
    }

    pub fn resolve_url(base_url: &str, href: &str) -> String {
        if href.starts_with("http://") || href.starts_with("https://") {
            return href.to_string();
        }

        let Some(scheme_end) = base_url.find("://") else {
            return href.to_string();
        };

        let host_end = base_url[scheme_end + 3..]
            .find('/')
            .map(|pos| scheme_end + 3 + pos)
            .unwrap_or(base_url.len());

        format!("{}{}", &base_url[..host_end], href)
    }

    fn perform_request(&self, method: &str, url: &str, body: &str, depth: u32) -> Option<String> {
        let m = Method::from_bytes(method.as_bytes()).ok()?;
        let result = self
            .client
            .request(m, url)
            .basic_auth(&self.config.username, Some(&self.config.password))
            .header("Content-Type", "application/xml; charset=utf-8")
            .header("Depth", depth.to_string())
            .body(body.to_string())
            .send();

        let response = match result {
            Ok(r) => r,
            Err(e) => {
                log::warn!("{} {} request error: {}", method, url, e);
                return None;
            }
        };

        let status = response.status();
        if !status.is_success() && status.as_u16() >= 400 {
            log::warn!("{} {} failed with status {}", method, url, status.as_u16());
            return None;
        }

        match response.text() {
            Ok(text) if !text.is_empty() => Some(text),
            Ok(_) => None,
            Err(e) => {
                log::warn!("{} {} failed to read body: {}", method, url, e);
                None
            }
        }
    }

    fn discover_principal(&self) -> String {
        let body = r#"<?xml version="1.0" encoding="utf-8" ?>
<d:propfind xmlns:d="DAV:">
  <d:prop>
    <d:current-user-principal />
  </d:prop>
</d:propfind>"#;

        let Some(xml) = self.perform_request("PROPFIND", &self.config.server_url, body, 0) else {
            return self.config.server_url.clone();
        };

        let Ok(doc) = roxmltree::Document::parse(&xml) else {
            log::warn!("Failed to parse current-user-principal response");
            return self.config.server_url.clone();
        };

        let root = doc.root();
        let Some(principal_node) = find_descendant_by_local_name(&root, "current-user-principal")
        else {
            return self.config.server_url.clone();
        };

        let Some(href_node) = find_child_by_local_name(&principal_node, "href") else {
            return self.config.server_url.clone();
        };

        let href = get_text_content(&href_node);
        if href.is_empty() {
            return self.config.server_url.clone();
        }

        Self::resolve_url(&self.config.server_url, &href)
    }

    pub fn discover_calendar_home_set(&self) -> Result<String, PointlessError> {
        let principal_url = self.discover_principal();

        let body = r#"<?xml version="1.0" encoding="utf-8" ?>
<d:propfind xmlns:d="DAV:" xmlns:c="urn:ietf:params:xml:ns:caldav">
  <d:prop>
    <c:calendar-home-set />
  </d:prop>
</d:propfind>"#;

        let xml = self.perform_request("PROPFIND", &principal_url, body, 0).ok_or_else(|| {
            PointlessError::General(format!(
                "CalDAV discovery failed: PROPFIND to {} returned no response",
                principal_url
            ))
        })?;

        let doc = roxmltree::Document::parse(&xml).map_err(|_| {
            PointlessError::General(format!(
                "CalDAV discovery failed: could not parse calendar-home-set response from {}",
                principal_url
            ))
        })?;

        let root = doc.root();
        let Some(home_set_node) = find_descendant_by_local_name(&root, "calendar-home-set") else {
            log::debug!("No calendar-home-set found, using server URL directly");
            return Ok(self.config.server_url.clone());
        };

        let Some(href_node) = find_child_by_local_name(&home_set_node, "href") else {
            log::debug!("No href in calendar-home-set, using server URL directly");
            return Ok(self.config.server_url.clone());
        };

        let href = get_text_content(&href_node);
        if href.is_empty() {
            return Ok(self.config.server_url.clone());
        }

        Ok(Self::resolve_url(&self.config.server_url, &href))
    }

    pub fn fetch_calendars(&self, home_set_url: &str) -> Vec<Calendar> {
        let body = r#"<?xml version="1.0" encoding="utf-8" ?>
<d:propfind xmlns:d="DAV:" xmlns:c="urn:ietf:params:xml:ns:caldav" xmlns:cs="http://apple.com/ns/ical/">
  <d:prop>
    <d:displayname />
    <d:resourcetype />
    <c:supported-calendar-component-set />
    <cs:calendar-color />
  </d:prop>
</d:propfind>"#;

        let Some(xml) = self.perform_request("PROPFIND", home_set_url, body, 1) else {
            return Vec::new();
        };

        let Ok(doc) = roxmltree::Document::parse(&xml) else {
            log::warn!("Failed to parse calendars response");
            return Vec::new();
        };

        let root = doc.root();
        let Some(multistatus) = find_descendant_by_local_name(&root, "multistatus") else {
            return Vec::new();
        };

        let mut calendars = Vec::new();

        for response in multistatus.children() {
            if !response.is_element() || local_name(response.tag_name().name()) != "response" {
                continue;
            }

            let Some(propstat) = find_descendant_by_local_name(&response, "propstat") else {
                continue;
            };

            let Some(prop) = find_child_by_local_name(&propstat, "prop") else {
                continue;
            };

            let Some(resourcetype) = find_child_by_local_name(&prop, "resourcetype") else {
                continue;
            };

            let is_calendar = resourcetype
                .children()
                .any(|child| child.is_element() && local_name(child.tag_name().name()) == "calendar");
            if !is_calendar {
                continue;
            }

            if let Some(sup_comp) =
                find_child_by_local_name(&prop, "supported-calendar-component-set")
            {
                let supports_vevent = sup_comp.children().any(|comp| {
                    comp.is_element()
                        && local_name(comp.tag_name().name()) == "comp"
                        && comp.attribute("name") == Some("VEVENT")
                });
                if !supports_vevent {
                    continue;
                }
            }

            let Some(href_node) = find_child_by_local_name(&response, "href") else {
                continue;
            };

            let id = get_text_content(&href_node);

            let title = find_child_by_local_name(&prop, "displayname")
                .map(|n| get_text_content(&n))
                .unwrap_or_default();

            let color = find_child_by_local_name(&prop, "calendar-color")
                .map(|n| normalize_color(&get_text_content(&n)))
                .unwrap_or_default();

            calendars.push(Calendar {
                id,
                title,
                color,
                writeable: false,
            });
        }

        calendars
    }

    pub fn fetch_events(
        &self,
        calendar_url: &str,
        calendar_id: &str,
        calendar_name: &str,
        range: &DateRange,
    ) -> Vec<CalendarEvent> {
        let time_range = format_time_range(range);
        let body = format!(
            r#"<?xml version="1.0" encoding="utf-8" ?>
<c:calendar-query xmlns:d="DAV:" xmlns:c="urn:ietf:params:xml:ns:caldav">
  <d:prop>
    <d:getetag />
    <c:calendar-data />
  </d:prop>
  <c:filter>
    <c:comp-filter name="VCALENDAR">
      <c:comp-filter name="VEVENT">
        <c:time-range {time_range} />
      </c:comp-filter>
    </c:comp-filter>
  </c:filter>
</c:calendar-query>"#
        );

        let Some(xml) = self.perform_request("REPORT", calendar_url, &body, 1) else {
            return Vec::new();
        };

        let Ok(doc) = roxmltree::Document::parse(&xml) else {
            log::warn!("Failed to parse events response");
            return Vec::new();
        };

        let root = doc.root();
        let Some(multistatus) = find_descendant_by_local_name(&root, "multistatus") else {
            return Vec::new();
        };

        let mut events = Vec::new();

        for response in multistatus.children() {
            if !response.is_element() || local_name(response.tag_name().name()) != "response" {
                continue;
            }

            let Some(cal_data) = find_descendant_by_local_name(&response, "calendar-data") else {
                continue;
            };

            let ical_data = get_text_content(&cal_data);
            if ical_data.is_empty() {
                continue;
            }

            let parsed = parse_ical_events(&ical_data, Some(range));
            for ev in parsed {
                events.push(CalendarEvent {
                    event_id: ev.uid,
                    calendar_id: calendar_id.to_string(),
                    calendar_name: calendar_name.to_string(),
                    title: ev.summary,
                    start_date: ev.dtstart,
                    end_date: ev.dtend,
                    is_all_day: ev.is_all_day,
                });
            }
        }

        events
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_resolve_url_absolute() {
        let result = CalDavClient::resolve_url("https://example.com/dav/", "https://other.com/path");
        assert_eq!(result, "https://other.com/path");
    }

    #[test]
    fn test_resolve_url_relative() {
        let result =
            CalDavClient::resolve_url("https://example.com/dav/", "/calendars/user/");
        assert_eq!(result, "https://example.com/calendars/user/");
    }

    #[test]
    fn test_resolve_url_no_scheme() {
        let result = CalDavClient::resolve_url("not-a-url", "/path");
        assert_eq!(result, "/path");
    }

    #[test]
    fn test_ensure_trailing_slash() {
        let mut url = "https://example.com".to_string();
        ensure_trailing_slash(&mut url);
        assert_eq!(url, "https://example.com/");

        let mut url2 = "https://example.com/".to_string();
        ensure_trailing_slash(&mut url2);
        assert_eq!(url2, "https://example.com/");

        let mut url3 = String::new();
        ensure_trailing_slash(&mut url3);
        assert_eq!(url3, "");
    }

    #[test]
    fn test_normalize_color() {
        assert_eq!(normalize_color("#FF0000FF"), "#FF0000");
        assert_eq!(normalize_color("#FF0000"), "#FF0000");
        assert_eq!(normalize_color("red"), "red");
    }

    #[test]
    fn test_calendar_home_set_parsing() {
        let xml = r#"<?xml version="1.0" encoding="utf-8" ?>
<d:multistatus xmlns:d="DAV:" xmlns:cal="urn:ietf:params:xml:ns:caldav">
  <d:response>
    <d:href>/principals/user/</d:href>
    <d:propstat>
      <d:prop>
        <cal:calendar-home-set>
          <d:href>/calendars/user/</d:href>
        </cal:calendar-home-set>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
</d:multistatus>"#;

        let doc = roxmltree::Document::parse(xml).unwrap();
        let root = doc.root();

        let home_set = find_descendant_by_local_name(&root, "calendar-home-set");
        assert!(home_set.is_some());

        let home_set = home_set.unwrap();
        let href = find_child_by_local_name(&home_set, "href");
        assert!(href.is_some());
        assert_eq!(get_text_content(&href.unwrap()), "/calendars/user/");
    }

    #[test]
    fn test_calendar_list_parsing() {
        let xml = r#"<?xml version="1.0" encoding="utf-8" ?>
<d:multistatus xmlns:d="DAV:" xmlns:cal="urn:ietf:params:xml:ns:caldav" xmlns:cs="http://apple.com/ns/ical/">
  <d:response>
    <d:href>/calendars/user/personal/</d:href>
    <d:propstat>
      <d:prop>
        <d:displayname>Personal</d:displayname>
        <d:resourcetype>
          <d:collection />
          <cal:calendar />
        </d:resourcetype>
        <cal:supported-calendar-component-set>
          <cal:comp name="VEVENT" />
        </cal:supported-calendar-component-set>
        <cs:calendar-color>#FF5733FF</cs:calendar-color>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
  <d:response>
    <d:href>/calendars/user/tasks/</d:href>
    <d:propstat>
      <d:prop>
        <d:displayname>Tasks</d:displayname>
        <d:resourcetype>
          <d:collection />
          <cal:calendar />
        </d:resourcetype>
        <cal:supported-calendar-component-set>
          <cal:comp name="VTODO" />
        </cal:supported-calendar-component-set>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
</d:multistatus>"#;

        let doc = roxmltree::Document::parse(xml).unwrap();
        let root = doc.root();
        let multistatus = find_descendant_by_local_name(&root, "multistatus").unwrap();

        let mut calendar_count = 0;
        let mut vevent_calendar_count = 0;

        for response in multistatus.children() {
            if !response.is_element() || local_name(response.tag_name().name()) != "response" {
                continue;
            }

            let Some(propstat) = find_descendant_by_local_name(&response, "propstat") else {
                continue;
            };

            let Some(prop) = find_child_by_local_name(&propstat, "prop") else {
                continue;
            };

            let Some(resourcetype) = find_child_by_local_name(&prop, "resourcetype") else {
                continue;
            };

            let is_calendar = resourcetype.children().any(|child| {
                child.is_element() && local_name(child.tag_name().name()) == "calendar"
            });
            if !is_calendar {
                continue;
            }
            calendar_count += 1;

            if let Some(sup_comp) =
                find_child_by_local_name(&prop, "supported-calendar-component-set")
            {
                let supports_vevent = sup_comp.children().any(|comp| {
                    comp.is_element()
                        && local_name(comp.tag_name().name()) == "comp"
                        && comp.attribute("name") == Some("VEVENT")
                });
                if supports_vevent {
                    vevent_calendar_count += 1;
                }
            }
        }

        assert_eq!(calendar_count, 2);
        assert_eq!(vevent_calendar_count, 1);
    }

    #[test]
    fn test_calendar_data_extraction() {
        let xml = r#"<?xml version="1.0" encoding="utf-8" ?>
<d:multistatus xmlns:d="DAV:" xmlns:cal="urn:ietf:params:xml:ns:caldav">
  <d:response>
    <d:href>/calendars/user/personal/event1.ics</d:href>
    <d:propstat>
      <d:prop>
        <d:getetag>"etag-123"</d:getetag>
        <cal:calendar-data>BEGIN:VCALENDAR
VERSION:2.0
BEGIN:VEVENT
UID:xml-ev-1
SUMMARY:Extracted Event
DTSTART:20250401T080000Z
DTEND:20250401T090000Z
END:VEVENT
END:VCALENDAR</cal:calendar-data>
      </d:prop>
      <d:status>HTTP/1.1 200 OK</d:status>
    </d:propstat>
  </d:response>
</d:multistatus>"#;

        let doc = roxmltree::Document::parse(xml).unwrap();
        let root = doc.root();

        let cal_data = find_descendant_by_local_name(&root, "calendar-data");
        assert!(cal_data.is_some());

        let ical_data = get_text_content(&cal_data.unwrap());
        assert!(!ical_data.is_empty());

        let events = parse_ical_events(&ical_data, None);
        assert_eq!(events.len(), 1);
        assert_eq!(events[0].uid, "xml-ev-1");
        assert_eq!(events[0].summary, "Extracted Event");
    }
}
