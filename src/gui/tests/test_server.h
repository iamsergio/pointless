// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../application.h"

#include <Spix/QtQmlBot.h>

namespace pointless {
class TestServer : public spix::TestServer
{
public:
    TestServer(int &argc, char **argv)
        : _app(argc, argv)
    {
        _bot = new spix::QtQmlBot();
        _bot->runTestServer(*this);
    }

    int exec()
    {
        return _app.exec();
    }

private:
    spix::QtQmlBot *_bot = nullptr;
    pointless::Application _app;
};

}
