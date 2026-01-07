// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <Spix/Data/ItemPath.h>
#include "../../3rdparty/spix/libs/Scenes/QtQuick/src/FindQtItem.h"

#include <QMetaObject>
#include <QQuickItem>

namespace SpixUtils {

inline QQuickItem *getListViewItemAtIndex(const spix::ItemPath &listViewPath, int index)
{
    auto listView = spix::qt::GetQQuickItemAtPath(listViewPath);
    if (!listView) {
        return nullptr;
    }

    QQuickItem *item = nullptr;

    const bool success = QMetaObject::invokeMethod(listView, "itemAtIndex",
                                                   Qt::BlockingQueuedConnection,
                                                   Q_RETURN_ARG(QQuickItem *, item),
                                                   Q_ARG(int, index));

    if (success) {
        return item;
    }
    return nullptr;
}

}
