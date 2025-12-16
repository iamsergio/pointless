// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data_provider.h"
#include "file_data_provider.h"
#include "supabase.h"
#include "context.h"
#include "test_supabase_provider.h"

std::unique_ptr<IDataProvider> IDataProvider::createProvider()
{
    const auto &context = pointless::core::Context::self();

    switch (context.dataProviderType) {
    case Type::None:
        std::abort();
    case Type::Local:
        return std::make_unique<FileDataProvider>(context.localFilePath);
    case Type::TestSupabase:
        return std::make_unique<TestSupabaseProvider>();
    case Type::Supabase:
        return SupabaseProvider::createDefault();
        break;
    }

    std::abort();
}
