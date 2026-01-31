// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/application.h"
#include "gui/tests/test_utils.h"

#include <Spix/QtQmlBot.h>
#include "../../../3rdparty/spix/libs/Scenes/QtQuick/src/Utils/DebugDump.h"

#include <QQuickWindow>
#include <QDebug>

namespace pointless {

class TestServer : public spix::TestServer
{
public:
    TestServer(int &argc, char **argv)
        : _app(argc, argv, "com.pointless.tests", initPlatform())
    {
        _bot = new spix::QtQmlBot();
        _bot->runTestServer(*this);
    }

    ~TestServer()
    {
        delete _bot;
    }

    int exec()
    {
        return _app.exec();
    }

    void dumpAllPaths()
    {
        auto windows = QGuiApplication::topLevelWindows();
        for (auto window : windows) {
            if (auto quickWindow = qobject_cast<QQuickWindow *>(window)) {
                qDebug() << "\n=== Dumping window: " << quickWindow->objectName() << " ===";
                spix::utils::DumpQQuickItemTree(quickWindow->contentItem());
            }
        }
    }

private:
    spix::QtQmlBot *_bot = nullptr;
    pointless::Application _app;
};

}
