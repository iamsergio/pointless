// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

class PomodoroController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_PROPERTY(QString currentTaskUuid READ currentTaskUuid NOTIFY currentTaskUuidChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(int remainingSeconds READ remainingSeconds NOTIFY remainingSecondsChanged)
    Q_PROPERTY(QString remainingTimeText READ remainingTimeText NOTIFY remainingSecondsChanged)
public:
    explicit PomodoroController(QObject *parent = nullptr);

    [[nodiscard]] QString currentTaskUuid() const;
    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] int remainingSeconds() const;
    [[nodiscard]] QString remainingTimeText() const;

    Q_INVOKABLE void play(const QString &taskUuid);
    Q_INVOKABLE void stop();

Q_SIGNALS:
    void currentTaskUuidChanged();
    void isRunningChanged();
    void remainingSecondsChanged();
    void pomodoroFinished();

private:
    void tick();
    static constexpr int PomodoroDurationSeconds = 30 * 60;
    QString _currentTaskUuid;
    int _remainingSeconds = 0;
    QTimer _timer;
};
