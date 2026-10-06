// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <Spix/TestServer.h>

#include <functional>
#include <memory>

namespace pointless::core {
class LocalData;
}

namespace pointless::tests {

class GuiTestHarness
{
public:
    static void setTestNow(int year, int month, int day, int hour, int minute);

    GuiTestHarness(int &argc, char **argv);
    ~GuiTestHarness();

    GuiTestHarness(const GuiTestHarness &) = delete;
    GuiTestHarness &operator=(const GuiTestHarness &) = delete;

    int run(spix::TestServer &test);
    void finish();

    core::LocalData &localData();
    void runOnGuiThread(const std::function<void()> &fn);
    void dumpTree();

private:
    struct Private;
    std::unique_ptr<Private> d;
};

}
