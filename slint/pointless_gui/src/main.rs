slint::include_modules!();

use std::rc::Rc;

use pointless_core::{Context, DataProviderType, LocalData};

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
    let all_tasks = local_data.data().get_all_tasks();

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
    window.on_toggle_done(|_uuid| {});
    window.run().unwrap();
}
