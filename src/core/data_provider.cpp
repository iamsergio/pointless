// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_provider.h"
#include "supabase.h"

static std::unique_ptr<IDataProvider> g_provider;

void IDataProvider::setProvider(std::unique_ptr<IDataProvider> provider)
{
    g_provider = std::move(provider);
}

std::unique_ptr<IDataProvider> IDataProvider::createProvider()
{
    if (g_provider) {
        return std::move(g_provider);
    }
    return Supabase::createDefault();
}
