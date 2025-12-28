// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_provider.h"
#include "supabase.h"
#include "context.h"

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
        std::abort();
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
        std::abort();
    case Type::Supabase:
        return SupabaseProvider::createDefault();
    }
#endif

    std::abort();
}
