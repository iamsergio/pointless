// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>

class IDataProvider
{
public:
    virtual ~IDataProvider() = default;

    virtual bool isAuthenticated() const = 0;
    virtual std::string retrieveData() = 0;
    virtual bool updateData(const std::string &data) = 0;

    static void setProvider(std::unique_ptr<IDataProvider> provider);
    static std::unique_ptr<IDataProvider> createProvider();
};
