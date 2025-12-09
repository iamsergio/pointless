// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_provider.h"
#include "supabase.h"

std::unique_ptr<IDataProvider> IDataProvider::s_provider;

void IDataProvider::setProvider(std::unique_ptr<IDataProvider> provider)
{
    s_provider = std::move(provider);
}

std::unique_ptr<IDataProvider> IDataProvider::createProvider()
{
    if (s_provider) {
        return std::move(s_provider);
    }
    return Supabase::createDefault();
}
