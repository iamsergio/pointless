// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <Spix/Data/ItemPath.h>
#include "../../3rdparty/spix/libs/Scenes/QtQuick/src/FindQtItem.h"

#include <QGuiApplication>
#include <QMetaObject>
#include <QQuickItem>
#include <QQuickWindow>

namespace SpixUtils {

inline QQuickItem *getItemAtPath(const spix::ItemPath &path)
{
    QQuickItem *result = nullptr;
    QMetaObject::invokeMethod(qApp, [&path, &result]() { result = spix::qt::GetQQuickItemAtPath(path); }, Qt::BlockingQueuedConnection);
    return result;
}

inline QQuickWindow *getWindowAtPath(const spix::ItemPath &path)
{
    QQuickWindow *result = nullptr;
    QMetaObject::invokeMethod(qApp, [&path, &result]() { result = spix::qt::GetQQuickWindowAtPath(path); }, Qt::BlockingQueuedConnection);
    return result;
}

inline QQuickItem *getListViewItemAtIndex(const spix::ItemPath &listViewPath, int index)
{
    auto listView = getItemAtPath(listViewPath);
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

inline QVariant getItemProperty(QObject *obj, const char *propertyName)
{
    QVariant result;
    QMetaObject::invokeMethod(obj, [obj, propertyName, &result]() { result = obj->property(propertyName); }, Qt::BlockingQueuedConnection);
    return result;
}

}
