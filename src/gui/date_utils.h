// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <optional>

class QDate;

namespace Gui::DateUtils {

QDate timepointToQDate(std::optional<std::chrono::system_clock::time_point> tp);

std::optional<std::chrono::system_clock::time_point> qdateToTimepoint(QDate date);

bool isMonday(QDate date);

QDate firstMondayOfWeek(QDate date);

QDate nextMonday(QDate date);

bool isOverdue(QDate dueDate, QDate currentDate);

bool isToday(QDate date);

} // namespace Gui::DateUtils
