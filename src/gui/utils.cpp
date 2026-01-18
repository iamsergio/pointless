// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "utils.h"

#include <QCoreApplication>

namespace Gui {

bool isAutoLogin()
{
#ifdef POINTLESS_DEVELOPER_MODE
    if (qApp) {
        return qApp->arguments().contains("--login");
    }
#endif
    return false;
}

}
