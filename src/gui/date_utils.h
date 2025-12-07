// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <optional>

class QDate;

namespace Gui::DateUtils {

QDate timepointToQDate(const std::optional<std::chrono::system_clock::time_point> &tp);

bool isMonday(QDate date);

QDate firstMondayOfWeek(QDate date);

bool isOverdue(QDate dueDate, QDate currentDate);

bool isToday(QDate date);

} // namespace Gui::DateUtils
