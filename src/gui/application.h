// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>

namespace pointless {

class Application : public QGuiApplication
{
    Q_OBJECT
public:
    enum class Option : std::uint8_t {
        None = 0,
        GuiTests = 1
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit Application(int &argc, char **argv, const QString &orgName, Options options = {});
    ~Application() override;

    Application(const Application &) = delete;
    Application(Application &&) = delete;
    Application &operator=(const Application &) = delete;
    Application &operator=(Application &&) = delete;

private:
    QQmlApplicationEngine _engine;
};

}
