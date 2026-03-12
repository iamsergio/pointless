// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT
slint::include_modules!();

use std::cell::RefCell;
use std::rc::Rc;

use chrono::{DateTime, Datelike, Duration, Local, NaiveDate, NaiveTime, TimeZone, Utc};
use pointless_core::tag::tag_is_builtin;
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

        let date_string = {
            let l = day_date.with_timezone(&Local);
            format!("{:04}-{:02}-{:02}", l.year(), l.month(), l.day())
        };

        let slint_tasks: Vec<TaskData> = day_tasks.iter().map(|t| task_to_slint(t)).collect();

        week_days.push(WeekDayData {
            pretty_date: pretty.into(),
            is_past,
            has_tasks,
            date_string: date_string.into(),
            tasks: Rc::new(slint::VecModel::from(slint_tasks)).into(),
        });
    }

    week_days
}

fn refresh_all_views(
    window: &MainWindow,
    local_data: &LocalData,
    current_monday: DateTime<Utc>,
) {
    let all_tasks = local_data.data().get_all_tasks();

    let week_days = build_week_days(current_monday, &all_tasks);
    window.set_week_days(Rc::new(slint::VecModel::from(week_days)).into());
    window.set_week_date_range_text(format_week_range(current_monday).into());

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

    window.set_soon_tasks(Rc::new(slint::VecModel::from(soon_tasks)).into());
    window.set_later_tasks(Rc::new(slint::VecModel::from(later_tasks)).into());
    window.set_goals_tasks(Rc::new(slint::VecModel::from(goals_tasks)).into());

    populate_tag_list(window, local_data);
}

fn populate_tag_list(window: &MainWindow, local_data: &LocalData) {
    let tags: Vec<TagDisplayData> = local_data
        .data()
        .all_tags()
        .iter()
        .filter(|t| !tag_is_builtin(&t.name))
        .map(|t| TagDisplayData {
            name: t.name.clone().into(),
            color: color_from_tag(&t.name),
        })
        .collect();
    window.set_available_tags(Rc::new(slint::VecModel::from(tags)).into());
}

fn build_calendar_days(
    year: i32,
    month: u32,
    selected_date: Option<DateTime<Utc>>,
) -> Vec<CalendarDayData> {
    let today_local = date_utils::today().with_timezone(&Local);
    let today_y = today_local.year();
    let today_m = today_local.month();
    let today_d = today_local.day();

    let first_of_month = NaiveDate::from_ymd_opt(year, month, 1).unwrap();
    let days_from_monday = first_of_month.weekday().num_days_from_monday();

    let start_date = first_of_month - Duration::days(days_from_monday as i64);

    let selected_local = selected_date.map(|d| d.with_timezone(&Local));
    let sel_y = selected_local.map(|d| d.year());
    let sel_m = selected_local.map(|d| d.month());
    let sel_d = selected_local.map(|d| d.day());

    let mut days = Vec::with_capacity(42);
    for i in 0..42 {
        let d = start_date + Duration::days(i);
        let is_current_month = d.month() == month && d.year() == year;
        let is_today = d.year() == today_y && d.month() == today_m && d.day() == today_d;
        let is_selected = sel_y == Some(d.year())
            && sel_m == Some(d.month())
            && sel_d == Some(d.day());
        let date_string = format!("{:04}-{:02}-{:02}", d.year(), d.month(), d.day());

        days.push(CalendarDayData {
            day_number: d.day() as i32,
            is_current_month,
            is_today,
            is_selected,
            date_string: date_string.into(),
        });
    }

    days
}

fn update_calendar(
    window: &MainWindow,
    year: i32,
    month: u32,
    selected_date: Option<DateTime<Utc>>,
) {
    let days = build_calendar_days(year, month, selected_date);
    window.set_calendar_days(Rc::new(slint::VecModel::from(days)).into());

    let month_names = [
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
    ];
    let label = format!("{} {}", month_names[(month - 1) as usize], year);
    window.set_calendar_month_label(label.into());
}

fn parse_date_string(s: &str) -> Option<DateTime<Utc>> {
    let parts: Vec<&str> = s.split('-').collect();
    if parts.len() != 3 {
        return None;
    }
    let year: i32 = parts[0].parse().ok()?;
    let month: u32 = parts[1].parse().ok()?;
    let day: u32 = parts[2].parse().ok()?;
    let naive = NaiveDate::from_ymd_opt(year, month, day)?;
    let midnight = NaiveTime::from_hms_opt(0, 0, 0).unwrap();
    let local_dt = Local
        .from_local_datetime(&naive.and_time(midnight))
        .single()?;
    Some(local_dt.with_timezone(&Utc))
}

struct EditState {
    uuid_being_edited: Option<String>,
    calendar_month: u32,
    calendar_year: i32,
    selected_date: Option<DateTime<Utc>>,
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
    let local_data = Rc::new(RefCell::new(local_data));

    let current_monday = Rc::new(RefCell::new(
        date_utils::this_weeks_monday(date_utils::today()),
    ));

    let window = MainWindow::new().unwrap();

    refresh_all_views(&window, &local_data.borrow(), *current_monday.borrow());

    // --- Week navigation ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_navigate_previous_week(move || {
            let new_monday = *monday.borrow() - Duration::days(7);
            *monday.borrow_mut() = new_monday;
            if let Some(w) = window_weak.upgrade() {
                refresh_all_views(&w, &ld.borrow(), new_monday);
            }
        });
    }

    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_navigate_next_week(move || {
            let new_monday = *monday.borrow() + Duration::days(7);
            *monday.borrow_mut() = new_monday;
            if let Some(w) = window_weak.upgrade() {
                refresh_all_views(&w, &ld.borrow(), new_monday);
            }
        });
    }

    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_navigate_today(move || {
            let new_monday = date_utils::this_weeks_monday(date_utils::today());
            *monday.borrow_mut() = new_monday;
            if let Some(w) = window_weak.upgrade() {
                refresh_all_views(&w, &ld.borrow(), new_monday);
            }
        });
    }

    // --- Toggle done ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_toggle_done(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.is_done = !task.is_done;
                if task.is_done {
                    task.completion_date = Some(chrono::Utc::now());
                } else {
                    task.completion_date = None;
                }
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Task clicked (context menu) ---
    {
        let window_weak = window.as_weak();
        let ld = local_data.clone();
        window.on_task_clicked(move |uuid| {
            let uuid_str = uuid.to_string();
            let ld_ref = ld.borrow();
            if let Some(task) = ld_ref.task_for_uuid(&uuid_str) {
                if let Some(w) = window_weak.upgrade() {
                    w.set_ctx_task_uuid(uuid.clone());
                    w.set_ctx_move_to_current_visible(!task.is_current());
                    w.set_ctx_move_to_soon_visible(!task.is_soon());
                    w.set_ctx_move_to_later_visible(!task.is_later());
                    w.set_ctx_move_to_tomorrow_visible(!task.is_due_tomorrow());
                    w.set_ctx_move_to_evening_visible(!task.is_evening() && task.is_current());
                    w.set_ctx_delete_visible(task.is_goal.unwrap_or(false));
                    w.set_context_menu_visible(true);
                }
            }
        });
    }

    // --- Context menu: Move to Current ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_move_to_current(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.remove_builtin_tags();
                task.add_tag("current");
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Move to Soon ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_move_to_soon(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.remove_builtin_tags();
                task.add_tag("soon");
                task.due_date = None;
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Move to Later ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_move_to_later(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.remove_builtin_tags();
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Move to Tomorrow ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_move_to_tomorrow(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.remove_builtin_tags();
                task.due_date = Some(date_utils::today() + Duration::days(1));
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Move to Next Monday ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_move_to_next_monday(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.remove_builtin_tags();
                task.due_date = Some(date_utils::next_monday(date_utils::today()));
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Move to Evening ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_move_to_evening(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            if let Some(mut task) = ld_ref.data().get_task(&uuid_str) {
                task.add_tag("evening");
                ld_ref.update_task(task);
                let _ = ld_ref.save();
            }
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Delete ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        window.on_context_delete(move |uuid| {
            let uuid_str = uuid.to_string();
            let mut ld_ref = ld.borrow_mut();
            ld_ref.remove_task(&uuid_str);
            let _ = ld_ref.save();
            if let Some(w) = window_weak.upgrade() {
                w.set_context_menu_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Context menu: Edit ---
    let edit_state = Rc::new(RefCell::new(EditState {
        uuid_being_edited: None,
        calendar_month: Local::now().month(),
        calendar_year: Local::now().year(),
        selected_date: None,
    }));

    {
        let window_weak = window.as_weak();
        let ld = local_data.clone();
        let es = edit_state.clone();
        window.on_context_edit(move |uuid| {
            let uuid_str = uuid.to_string();
            let ld_ref = ld.borrow();
            if let Some(task) = ld_ref.task_for_uuid(&uuid_str) {
                if let Some(w) = window_weak.upgrade() {
                    w.set_context_menu_visible(false);
                    w.set_edit_task_title(task.title.clone().into());

                    let user_tag = task.tag_name();
                    w.set_edit_task_tag(user_tag.into());
                    w.set_edit_task_evening(task.is_evening());
                    w.set_edit_task_is_goal(task.is_goal.unwrap_or(false));
                    w.set_edit_task_notes(
                        task.description.clone().unwrap_or_default().into(),
                    );

                    let mut es_ref = es.borrow_mut();
                    es_ref.uuid_being_edited = Some(uuid_str);
                    es_ref.selected_date = task.due_date;

                    if let Some(due) = task.due_date {
                        let local = due.with_timezone(&Local);
                        es_ref.calendar_month = local.month();
                        es_ref.calendar_year = local.year();
                    } else {
                        es_ref.calendar_month = Local::now().month();
                        es_ref.calendar_year = Local::now().year();
                    }

                    update_calendar(
                        &w,
                        es_ref.calendar_year,
                        es_ref.calendar_month,
                        es_ref.selected_date,
                    );
                    w.set_edit_task_visible(true);
                }
            }
        });
    }

    // --- Context menu: Edit Notes ---
    {
        let window_weak = window.as_weak();
        let ld = local_data.clone();
        let es = edit_state.clone();
        window.on_context_edit_notes(move |uuid| {
            let uuid_str = uuid.to_string();
            let ld_ref = ld.borrow();
            if let Some(task) = ld_ref.task_for_uuid(&uuid_str) {
                if let Some(w) = window_weak.upgrade() {
                    w.set_context_menu_visible(false);
                    w.set_edit_task_title(task.title.clone().into());

                    let user_tag = task.tag_name();
                    w.set_edit_task_tag(user_tag.into());
                    w.set_edit_task_evening(task.is_evening());
                    w.set_edit_task_is_goal(task.is_goal.unwrap_or(false));
                    w.set_edit_task_notes(
                        task.description.clone().unwrap_or_default().into(),
                    );

                    let mut es_ref = es.borrow_mut();
                    es_ref.uuid_being_edited = Some(uuid_str);
                    es_ref.selected_date = task.due_date;

                    if let Some(due) = task.due_date {
                        let local = due.with_timezone(&Local);
                        es_ref.calendar_month = local.month();
                        es_ref.calendar_year = local.year();
                    } else {
                        es_ref.calendar_month = Local::now().month();
                        es_ref.calendar_year = Local::now().year();
                    }

                    update_calendar(
                        &w,
                        es_ref.calendar_year,
                        es_ref.calendar_month,
                        es_ref.selected_date,
                    );
                    w.set_edit_task_visible(true);
                }
            }
        });
    }

    // --- Add task button (global "+") ---
    {
        let window_weak = window.as_weak();
        let es = edit_state.clone();
        window.on_add_task_clicked(move || {
            if let Some(w) = window_weak.upgrade() {
                w.set_edit_task_title("".into());
                w.set_edit_task_tag("".into());
                w.set_edit_task_evening(false);
                w.set_edit_task_is_goal(false);
                w.set_edit_task_notes("".into());

                let mut es_ref = es.borrow_mut();
                es_ref.uuid_being_edited = None;
                es_ref.selected_date = None;
                es_ref.calendar_month = Local::now().month();
                es_ref.calendar_year = Local::now().year();

                update_calendar(
                    &w,
                    es_ref.calendar_year,
                    es_ref.calendar_month,
                    es_ref.selected_date,
                );
                w.set_edit_task_visible(true);
            }
        });
    }

    // --- Add task for day (week view "+") ---
    {
        let window_weak = window.as_weak();
        let es = edit_state.clone();
        let monday = current_monday.clone();
        window.on_add_task_for_day(move |date_str| {
            if let Some(w) = window_weak.upgrade() {
                w.set_edit_task_title("".into());
                w.set_edit_task_tag("".into());
                w.set_edit_task_evening(false);
                w.set_edit_task_is_goal(false);
                w.set_edit_task_notes("".into());

                let mut es_ref = es.borrow_mut();
                es_ref.uuid_being_edited = None;
                es_ref.selected_date = parse_date_string(&date_str.to_string());

                if let Some(d) = es_ref.selected_date {
                    let local = d.with_timezone(&Local);
                    es_ref.calendar_month = local.month();
                    es_ref.calendar_year = local.year();
                } else {
                    es_ref.calendar_month = Local::now().month();
                    es_ref.calendar_year = Local::now().year();
                }

                update_calendar(
                    &w,
                    es_ref.calendar_year,
                    es_ref.calendar_month,
                    es_ref.selected_date,
                );
                w.set_edit_task_visible(true);
            }
            let _ = &monday;
        });
    }

    // --- Calendar navigation ---
    {
        let window_weak = window.as_weak();
        let es = edit_state.clone();
        window.on_calendar_previous_month(move || {
            let mut es_ref = es.borrow_mut();
            if es_ref.calendar_month == 1 {
                es_ref.calendar_month = 12;
                es_ref.calendar_year -= 1;
            } else {
                es_ref.calendar_month -= 1;
            }
            if let Some(w) = window_weak.upgrade() {
                update_calendar(
                    &w,
                    es_ref.calendar_year,
                    es_ref.calendar_month,
                    es_ref.selected_date,
                );
            }
        });
    }

    {
        let window_weak = window.as_weak();
        let es = edit_state.clone();
        window.on_calendar_next_month(move || {
            let mut es_ref = es.borrow_mut();
            if es_ref.calendar_month == 12 {
                es_ref.calendar_month = 1;
                es_ref.calendar_year += 1;
            } else {
                es_ref.calendar_month += 1;
            }
            if let Some(w) = window_weak.upgrade() {
                update_calendar(
                    &w,
                    es_ref.calendar_year,
                    es_ref.calendar_month,
                    es_ref.selected_date,
                );
            }
        });
    }

    // --- Calendar day selected ---
    {
        let window_weak = window.as_weak();
        let es = edit_state.clone();
        window.on_calendar_day_selected(move |date_str| {
            let mut es_ref = es.borrow_mut();
            let new_date = parse_date_string(&date_str.to_string());

            if es_ref.selected_date.is_some()
                && new_date.is_some()
                && es_ref.selected_date == new_date
            {
                es_ref.selected_date = None;
            } else {
                es_ref.selected_date = new_date;
            }

            if let Some(w) = window_weak.upgrade() {
                update_calendar(
                    &w,
                    es_ref.calendar_year,
                    es_ref.calendar_month,
                    es_ref.selected_date,
                );
            }
        });
    }

    // --- Save task ---
    {
        let window_weak = window.as_weak();
        let monday = current_monday.clone();
        let ld = local_data.clone();
        let es = edit_state.clone();
        window.on_save_task(move |title, tag, evening, is_goal, notes| {
            let title_str = title.to_string().trim().to_string();
            if title_str.is_empty() {
                return;
            }

            let es_ref = es.borrow();
            let mut ld_ref = ld.borrow_mut();

            let is_new = es_ref.uuid_being_edited.is_none();

            let mut task = if let Some(ref uuid) = es_ref.uuid_being_edited {
                match ld_ref.data().get_task(uuid) {
                    Some(t) => t,
                    None => return,
                }
            } else {
                let uuid = uuid::Uuid::new_v4().to_string();
                pointless_core::Task::new(uuid, chrono::Utc::now(), title_str.clone())
            };

            task.title = title_str;
            task.description = if notes.is_empty() {
                None
            } else {
                Some(notes.to_string())
            };
            task.is_goal = if is_goal { Some(true) } else { None };
            task.due_date = es_ref.selected_date;

            let user_tag = tag.to_string();
            task.remove_builtin_tags();
            let non_builtin_tags: Vec<String> = task
                .tags
                .iter()
                .filter(|t| !tag_is_builtin(t))
                .cloned()
                .collect();

            task.tags.clear();

            if !user_tag.is_empty() {
                task.add_tag(&user_tag);
            } else {
                for t in &non_builtin_tags {
                    task.add_tag(t);
                }
            }

            if evening {
                task.add_tag("evening");
            }

            if is_new {
                if let Some(w) = window_weak.upgrade() {
                    let view = w.get_current_view();
                    match view {
                        ViewType::Soon => {
                            task.add_tag("soon");
                        }
                        ViewType::Later => {}
                        ViewType::Goals => {
                            task.is_goal = Some(true);
                        }
                        _ => {
                            if task.due_date.is_none() {
                                task.add_tag("current");
                            }
                        }
                    }
                }
                ld_ref.add_task(task);
            } else {
                ld_ref.update_task(task);
            }

            let _ = ld_ref.save();

            if let Some(w) = window_weak.upgrade() {
                w.set_edit_task_visible(false);
                refresh_all_views(&w, &ld_ref, *monday.borrow());
            }
        });
    }

    // --- Edit task back ---
    {
        let window_weak = window.as_weak();
        window.on_edit_task_back(move || {
            if let Some(w) = window_weak.upgrade() {
                w.set_edit_task_visible(false);
            }
        });
    }

    window.run().unwrap();
}
