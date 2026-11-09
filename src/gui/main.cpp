// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"

#include <algorithm>
#include <span>
#include <string_view>

namespace {
bool isCommandLineOnly(std::span<char *> args)
{
    return std::ranges::any_of(args, [](std::string_view arg) {
        return arg == "--add" || arg.starts_with("--add=");
    });
}
}

int main(int argc, char *argv[])
{
    // No window is shown, so don't require a display server
    if (isCommandLineOnly(std::span(argv, static_cast<std::size_t>(argc))))
        qputenv("QT_QPA_PLATFORM", "offscreen");

    pointless::Application app(argc, argv, QStringLiteral("com.iamsergio"));
    return pointless::Application::exec();
}
