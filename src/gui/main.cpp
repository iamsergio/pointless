// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "application.h"

int main(int argc, char *argv[])
{
    pointless::Application app(argc, argv, QStringLiteral("com.iamsergio"));
    return pointless::Application::exec();
}
