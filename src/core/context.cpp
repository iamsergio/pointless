// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "context.h"

using namespace pointless::core;

namespace {
Context s_currentContext; // NOLINT
}

void Context::setContext(const Context &context)
{
    s_currentContext = context;
}

Context Context::self()
{
    return s_currentContext;
}
