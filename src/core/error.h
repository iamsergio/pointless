// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <expected>
#include <source_location>
#include <string>
#include <vector>
#include <iostream>

struct ErrorNode
{
    std::string message;
    std::source_location location;
};

struct TraceableError
{
    std::vector<ErrorNode> stack;

    static std::unexpected<TraceableError> create(std::string msg,
                                                  std::source_location loc = std::source_location::current())
    {
        return std::unexpected(TraceableError { { { .message = std::move(msg), .location = loc } } });
    }

    static std::unexpected<TraceableError> create(std::string msg,
                                                  const TraceableError &other,
                                                  std::source_location loc = std::source_location::current())
    {
        TraceableError error { { { .message = std::move(msg), .location = loc } } };
        error.consume(other);
        return std::unexpected(std::move(error));
    }

    explicit operator std::string() const
    {
        std::string result;
        for (size_t i = 0; i < stack.size(); ++i) {
            if (i > 0)
                result += "\n";
            result += stack[i].message;
        }
        return result;
    }

    [[nodiscard]] std::string toString() const
    {
        return static_cast<std::string>(*this);
    }

private:
    TraceableError &consume(const TraceableError &other)
    {
        stack.insert(stack.end(),
                     std::make_move_iterator(other.stack.begin()),
                     std::make_move_iterator(other.stack.end()));
        return *this;
    }
};

inline std::ostream &operator<<(std::ostream &os, const TraceableError &error)
{
    os << "TraceableError(\n";
    for (size_t i = 0; i < error.stack.size(); ++i) {
        const auto &node = error.stack[i];
        os << "  [" << i << "] " << node.message << "\n";
        os << "      at " << node.location.file_name() << ":" << node.location.line() << "\n";
    }
    os << ")";
    return os;
}
