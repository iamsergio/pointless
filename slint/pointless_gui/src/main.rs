// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT
slint::include_modules!();

use std::cell::RefCell;
use std::rc::Rc;

use chrono::{DateTime, Datelike, Duration, Local, Utc};
use pointless_core::{date_utils, Context, DataProviderType, LocalData};

fn color_from_tag(tag_name: &str) -> slint::Color {
    let colors = [
        slint::Color::from_argb_u8(0xff, 0x2E, 0xCC, 0x71),
        slint::Color::from_argb_u8(0xff, 0x34, 0x98, 0xDB),
        slint::Color::from_argb_u8(0xff, 0x9B, 0x59, 0x34),
        slint::Color::from_argb_u8(0xff, 0xFF, 0x8A, 0x00),
    ];
    if tag_name.is_empty() {
        return slint::Color::from_argb_u8(0xff, 0x55, 0x55, 0x55);
    }
    let hash = tag_name
        .bytes()
        .fold(0u64, |acc, b| acc.wrapping_mul(31).wrapping_add(b as u64));
    colors[(hash as usize) % colors.len()]
}

fn format_due_date(task: &pointless_core::Task) -> String {
    match task.due_date {
        Some(d) => {
            let local = d.with_timezone(&chrono::Local);
            local.format("%b %d").to_string()
        }
        None => String::new(),
    }
}

fn task_to_slint(task: &pointless_core::Task) -> TaskData {
    TaskData {
        uuid: task.uuid.clone().into(),
        title: task.title.clone().into(),
        is_done: task.is_done,
        is_important: task.is_important,
        is_evening: task.is_evening(),
        due_date: format_due_date(task).into(),
        has_due_date: task.due_date.is_some(),
        tag_name: task.tag_name().into(),
        tag_color: color_from_tag(&task.tag_name()),
        has_notes: task
            .description
            .as_ref()
            .map(|d| !d.is_empty())
            .unwrap_or(false),
        is_goal: task.is_goal.unwrap_or(false),
    }
}

fn format_week_range(monday: DateTime<Utc>) -> String {
    let sunday = monday + Duration::days(6);
    let mon_local = monday.with_timezone(&Local);
    let sun_local = sunday.with_timezone(&Local);
    format!(
        "{} {} - {} {}",
        mon_local.format("%b"),
        mon_local.day(),
        sun_local.format("%b"),
        sun_local.day()
    )
}

fn tasks_for_day(
    day_date: DateTime<Utc>,
    all_tasks: &[pointless_core::Task],
) -> Vec<&pointless_core::Task> {
    let today = date_utils::today();
    let is_today = day_date == today;

    let mut day_tasks: Vec<&pointless_core::Task> = all_tasks
        .iter()
        .filter(|t| {
            if t.is_done {
                return false;
            }
            if is_today {
                if t.is_overdue() {
                    return true;
                }
                if t.is_current() && t.due_date.is_none() {
                    return true;
                }
            }
            match t.due_date {
                Some(due) => {
                    let trimmed = date_utils::trim_time(due);
                    if trimmed == day_date {
                        if !is_today && t.is_overdue() {
                            return false;
                        }
                        return true;
                    }
                    false
                }
                None => false,
            }
        })
        .collect();

    day_tasks.sort_by(|a, b| {
        let imp_cmp = b.is_important.cmp(&a.is_important);
        if imp_cmp != std::cmp::Ordering::Equal {
            return imp_cmp;
        }
        let eve_cmp = a.is_evening().cmp(&b.is_evening());
        if eve_cmp != std::cmp::Ordering::Equal {
            return eve_cmp;
        }
        a.tag_name().cmp(&b.tag_name())
    });

    day_tasks
}

fn build_week_days(
    monday: DateTime<Utc>,
    all_tasks: &[pointless_core::Task],
) -> Vec<WeekDayData> {
    let today = date_utils::today();
    let mut week_days = Vec::new();

    for i in 0..7 {
        let day_date = monday + Duration::days(i);
        let is_past = day_date < today;
        let day_tasks = tasks_for_day(day_date, all_tasks);
        let has_tasks = !day_tasks.is_empty();

        if is_past && !has_tasks {
            continue;
        }

        let local = day_date.with_timezone(&Local);
        let pretty = format!(
            "{}, {}",
            date_utils::weekday_name(day_date).to_uppercase(),
            local.day()
        );

        let slint_tasks: Vec<TaskData> = day_tasks.iter().map(|t| task_to_slint(t)).collect();

        week_days.push(WeekDayData {
            pretty_date: pretty.into(),
            is_past,
            has_tasks,
            tasks: Rc::new(slint::VecModel::from(slint_tasks)).into(),
        });
    }

    week_days
}

fn update_week_view(
    window: &MainWindow,
    monday: DateTime<Utc>,
    all_tasks: &[pointless_core::Task],
) {
    let week_days = build_week_days(monday, all_tasks);
    window.set_week_days(Rc::new(slint::VecModel::from(week_days)).into());
    window.set_week_date_range_text(format_week_range(monday).into());
}

fn main() {
    let data_dir = std::env::var("POINTLESS_CLIENT_DATA_DIR").unwrap_or_else(|_| {
        let home = std::env::var("HOME").unwrap_or_default();
        format!("{}/.config/pointless", home)
    });
    let data_file = format!("{}/pointless.json", data_dir);

    Context::set_context(Context::new(
        DataProviderType::TestsLocal,
        data_file,
        0,
        false,
    ));

    let mut local_data = LocalData::new();
    let _ = local_data.load_data_from_file();
    let all_tasks = Rc::new(local_data.data().get_all_tasks());

    let soon_tasks: Vec<TaskData> = all_tasks
        .iter()
        .filter(|t| !t.is_done && t.is_soon())
        .map(task_to_slint)
        .collect();

    let later_tasks: Vec<TaskData> = all_tasks
        .iter()
        .filter(|t| !t.is_done && t.is_later())
        .map(task_to_slint)
        .collect();

    let goals_tasks: Vec<TaskData> = all_tasks
        .iter()
        .filter(|t| !t.is_done && t.is_goal.unwrap_or(false))
        .map(task_to_slint)
        .collect();

    let window = MainWindow::new().unwrap();
    window.set_soon_tasks(Rc::new(slint::VecModel::from(soon_tasks)).into());
    window.set_later_tasks(Rc::new(slint::VecModel::from(later_tasks)).into());
    window.set_goals_tasks(Rc::new(slint::VecModel::from(goals_tasks)).into());

    let current_monday = Rc::new(RefCell::new(
        date_utils::this_weeks_monday(date_utils::today()),
    ));

    update_week_view(&window, *current_monday.borrow(), &all_tasks);

    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let tasks = all_tasks.clone();
        window.on_navigate_previous_week(move || {
            let new_monday = *monday.borrow() - Duration::days(7);
            *monday.borrow_mut() = new_monday;
            if let Some(w) = window_weak.upgrade() {
                update_week_view(&w, new_monday, &tasks);
            }
        });
    }

    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let tasks = all_tasks.clone();
        window.on_navigate_next_week(move || {
            let new_monday = *monday.borrow() + Duration::days(7);
            *monday.borrow_mut() = new_monday;
            if let Some(w) = window_weak.upgrade() {
                update_week_view(&w, new_monday, &tasks);
            }
        });
    }

    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let tasks = all_tasks.clone();
        window.on_navigate_today(move || {
            let new_monday = date_utils::this_weeks_monday(date_utils::today());
            *monday.borrow_mut() = new_monday;
            if let Some(w) = window_weak.upgrade() {
                update_week_view(&w, new_monday, &tasks);
            }
        });
    }

    window.on_toggle_done(|_uuid| {});
    window.on_add_task_for_day(|_idx| {});
    window.run().unwrap();
}
