// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_provider.h"
#include "supabase.h"
#include "context.h"
#include "utils.h"

#ifdef POINTLESS_ENABLE_TESTS
#include "test_local_provider.h"
#include "test_supabase_provider.h"
#endif

std::unique_ptr<IDataProvider> IDataProvider::createProvider()
{
    const auto &context = pointless::core::Context::self();

#ifdef POINTLESS_ENABLE_TESTS
    switch (context.dataProviderType()) {
    case Type::None:
        pointless::abort("invalid IDataProvider::Type enum value");
        break;
    case Type::TestsLocal:
        return std::make_unique<TestLocalDataProvider>(context.localFilePath());
    case Type::TestSupabase:
        return std::make_unique<TestSupabaseProvider>();
    case Type::Supabase:
        return SupabaseProvider::createDefault();
    }
#else
    switch (context.dataProviderType()) {
    case Type::TestSupabase:
    case Type::TestsLocal:
    case Type::None:
        pointless::abort("invalid IDataProvider::Type enum value");
        break;
    case Type::Supabase:
        return SupabaseProvider::createDefault();
    }
#endif

    pointless::abort("invalid IDataProvider::Type enum value");
    return {};
}
