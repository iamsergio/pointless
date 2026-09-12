// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/logger.h"

#include <QtGlobal>
#include <QString>
#include <QList>

#include <cstdlib>

namespace pointless::gui {

inline bool isWhiteListed(const QString &msg)
{
#ifdef Q_OS_APPLE
    // We mostly want this for the tests
    return true;
#endif

    static QStringList whiteList = {
        "Populating font family aliases took",
        "DelegateModel::cancel:",
        // Spurious EINVAL from the event loop's poll() when a debugger/injector
        // (e.g. qt-commander) ptrace-attaches while the main thread is blocked
        // in it -- not indicative of an actual bug, and Qt itself treats this
        // as non-fatal by default (QT_CONFIG(poll_exit_on_error) is off).
        "qt_safe_poll"
    };

    return std::ranges::any_of(whiteList.begin(), whiteList.end(), [&](const QString &entry) {
        return msg.contains(entry);
    });
}

inline void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const char *file = (context.file != nullptr) ? context.file : "";
    const char *function = (context.function != nullptr) ? context.function : "";

    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
        P_LOG_INFO("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtWarningMsg:
        P_LOG_WARNING_NOABORT("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtCriticalMsg:
        P_LOG_CRITICAL_NOABORT("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtFatalMsg:
        P_LOG_ERROR("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        std::abort();
        break;
    }

#ifdef POINTLESS_DEVELOPER_MODE
    static const bool disabled = std::getenv("POINTLESS_DISABLE_FATAL_WARNINGS") != nullptr;
    if (!disabled && !isWhiteListed(msg) && (type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg)) {
        std::abort();
    }
#endif
}
}
