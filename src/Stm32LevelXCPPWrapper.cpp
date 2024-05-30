/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Stm32LevelXCPPWrapper.hpp"
#include <Stm32LevelX.hpp>

UINT Stm32LevelX_setup(TX_BYTE_POOL *byte_pool) {
    return Stm32LevelX::LevelX::setup(byte_pool);
}
