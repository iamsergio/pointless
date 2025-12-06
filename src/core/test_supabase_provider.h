// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "supabase.h"

class TestSupabaseProvider : public Supabase
{
public:
    using Supabase::Supabase;

    std::pair<std::string, std::string> defaultLoginPassword() const override;
};
