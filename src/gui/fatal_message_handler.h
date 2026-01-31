// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/logger.h"

#include <QtGlobal>
#include <QString>

#include <cstdlib>

namespace pointless::gui {
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
    if (type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg) {
        std::abort();
    }
#endif
}
}
