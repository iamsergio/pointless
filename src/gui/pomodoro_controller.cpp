// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "pomodoro_controller.h"

PomodoroController::PomodoroController(QObject *parent)
    : QObject(parent)
{
    _timer.setInterval(1000);
    connect(&_timer, &QTimer::timeout, this, &PomodoroController::tick);
}

QString PomodoroController::currentTaskUuid() const
{
    return _currentTaskUuid;
}

bool PomodoroController::isRunning() const
{
    return _timer.isActive();
}

bool PomodoroController::isRunningThisTask(const pointless::core::Task &task) const
{
    return isRunningThisTask(QString::fromStdString(task.uuid));
}

bool PomodoroController::isRunningThisTask(const QString &taskUuid) const
{
    return isRunning() && _currentTaskUuid == taskUuid;
}

int PomodoroController::remainingSeconds() const
{
    return _remainingSeconds;
}

QString PomodoroController::remainingTimeText() const
{
    const int minutes = _remainingSeconds / 60;
    const int seconds = _remainingSeconds % 60;
    return QStringLiteral("%1:%2").arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
}

void PomodoroController::play(const QString &taskUuid)
{
    if (taskUuid.isEmpty())
        return;

    const bool wasRunning = isRunning();
    const QString oldUuid = _currentTaskUuid;

    _currentTaskUuid = taskUuid;
    _remainingSeconds = PomodoroDurationSeconds;
    _timer.start();

    if (oldUuid != _currentTaskUuid)
        Q_EMIT currentTaskUuidChanged();
    if (!wasRunning)
        Q_EMIT isRunningChanged();
    Q_EMIT remainingSecondsChanged();
}

void PomodoroController::stop()
{
    if (_currentTaskUuid.isEmpty())
        return;

    _timer.stop();
    _currentTaskUuid.clear();
    _remainingSeconds = 0;

    Q_EMIT currentTaskUuidChanged();
    Q_EMIT isRunningChanged();
    Q_EMIT remainingSecondsChanged();
}

void PomodoroController::tick()
{
    if (_remainingSeconds <= 0)
        return;

    --_remainingSeconds;
    Q_EMIT remainingSecondsChanged();

    if (_remainingSeconds == 0) {
        _timer.stop();
        const QString uuid = _currentTaskUuid;
        _currentTaskUuid.clear();
        Q_EMIT currentTaskUuidChanged();
        Q_EMIT isRunningChanged();
        Q_EMIT pomodoroFinished();
    }
}
