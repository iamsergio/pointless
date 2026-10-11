// SPDX-FileCopyrightText: 2026 Sergio Martins
// SPDX-License-Identifier: MIT

#include "bind.h"

#include "gui/application.h"
#include "gui/gui_controller.h"

int main(int argc, char *argv[])
{
    pointless::Application app(argc, argv, QStringLiteral("com.iamsergio"));

    auto window = ui::App::create();
    kdab::qtslintbridge::Bridge bridge;
    bindPointless(bridge, *window, *GuiController::instance());

    window->run();
    return 0;
}
