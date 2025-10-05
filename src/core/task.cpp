// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT



#include "task.h"
#include <expected>

namespace PointlessCore {

Task::Task()
{
}

Task::Task(const std::string &uuid_, const std::chrono::system_clock::time_point &creationTimestamp_, const std::string &title_)
    : uuid(uuid_)
    , title(title_)
    , creationTimestamp(creationTimestamp_)
{
}

nlohmann::json Task::toJson() const
{
    nlohmann::json j;
    j["uuid"] = uuid;
    if (parentUuid)
        j["parentUuid"] = *parentUuid;
    j["title"] = title;
    j["isDone"] = isDone;
    j["isImportant"] = isImportant;
    j["hideOnWeekends"] = hideOnWeekends;
    j["tags"] = nlohmann::json::array();
    for (const auto &tag : tags)
        j["tags"].push_back(tag.name());
    j["creationTimestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(creationTimestamp.time_since_epoch()).count();
    if (dueDate)
        j["dueDate"] = std::chrono::duration_cast<std::chrono::milliseconds>(dueDate->time_since_epoch()).count();
    if (completionDate)
        j["completionDate"] = std::chrono::duration_cast<std::chrono::milliseconds>(completionDate->time_since_epoch()).count();
    if (modificationTimestamp)
        j["modificationTimestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(modificationTimestamp->time_since_epoch()).count();
    if (uuidInDeviceCalendar)
        j["uuidInDeviceCalendar"] = *uuidInDeviceCalendar;
    if (deviceCalendarUuid)
        j["deviceCalendarUuid"] = *deviceCalendarUuid;
    if (deviceCalendarName)
        j["deviceCalendarName"] = *deviceCalendarName;
    return j;
}

std::expected<Task, std::string> Task::fromJson(const nlohmann::json &j)
{
    Task task;
    try {
        task.uuid = j.at("uuid").get<std::string>();
        if (j.contains("parentUuid"))
            task.parentUuid = j.at("parentUuid").get<std::string>();
        task.title = j.value("title", "");
        task.isDone = j.value("isDone", false);
        task.isImportant = j.value("isImportant", false);
        task.hideOnWeekends = j.value("hideOnWeekends", false);
        if (j.contains("tags")) {
            for (const auto &tagName : j["tags"]) {
                task.tags.emplace_back(tagName.get<std::string>());
            }
        }
        task.creationTimestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(j.at("creationTimestamp").get<int64_t>()));
        if (j.contains("dueDate"))
            task.dueDate = std::chrono::system_clock::time_point(std::chrono::milliseconds(j["dueDate"].get<int64_t>()));
        if (j.contains("completionDate"))
            task.completionDate = std::chrono::system_clock::time_point(std::chrono::milliseconds(j["completionDate"].get<int64_t>()));
        if (j.contains("modificationTimestamp"))
            task.modificationTimestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(j["modificationTimestamp"].get<int64_t>()));
        if (j.contains("uuidInDeviceCalendar"))
            task.uuidInDeviceCalendar = j["uuidInDeviceCalendar"].get<std::string>();
        if (j.contains("deviceCalendarUuid"))
            task.deviceCalendarUuid = j["deviceCalendarUuid"].get<std::string>();
        if (j.contains("deviceCalendarName"))
            task.deviceCalendarName = j["deviceCalendarName"].get<std::string>();
        return task;
    } catch (const std::exception& e) {
        return std::unexpected<std::string>(std::string("Task::fromJson error: ") + e.what());
    }
}

} // namespace PointlessCore
