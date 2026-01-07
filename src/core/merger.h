// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data.h"

namespace pointless::core {

DataPayload merge(const DataPayload &localData, const DataPayload &serverData);

}
