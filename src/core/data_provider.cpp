// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_provider.h"
#include "supabase.h"
#include "context.h"
#include "test_local_provider.h"
#include "test_supabase_provider.h"

std::unique_ptr<IDataProvider> IDataProvider::createProvider()
{
    const auto &context = pointless::core::Context::self();

    switch (context.dataProviderType) {
    case Type::None:
        std::abort();
    case Type::TestsLocal:
#ifdef POINTLESS_ENABLE_TESTS
        return std::make_unique<TestLocalDataProvider>(context.localFilePath);
#else
        std::abort();
#endif
    case Type::TestSupabase:
#ifdef POINTLESS_ENABLE_TESTS
        return std::make_unique<TestSupabaseProvider>();
#else
        std::abort();
#endif
    case Type::Supabase:
        return SupabaseProvider::createDefault();
    }

    std::abort();
}
