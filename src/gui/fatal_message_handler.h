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
        "Populating font family aliases took"
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
        P_LOG_DEBUG("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtInfoMsg:
        P_LOG_INFO("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtWarningMsg:
        P_LOG_WARNING("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtCriticalMsg:
        P_LOG_ERROR("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        break;
    case QtFatalMsg:
        P_LOG_ERROR("{} ({}:{}, {})", msg.toStdString(), file, context.line, function);
        std::abort();
        break;
    }

#ifdef POINTLESS_DEVELOPER_MODE
    if (!isWhiteListed(msg) && (type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg)) {
        std::abort();
    }
#endif
}
}
