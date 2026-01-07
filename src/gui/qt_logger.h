// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QString>

#include <spdlog/fmt/fmt.h>

template<>
struct fmt::formatter<QString> : fmt::formatter<std::string>
{
    auto format(const QString &str, format_context &ctx) const -> decltype(ctx.out())
    {
        return fmt::formatter<std::string>::format(str.toStdString(), ctx);
    }
};
