/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_ABSTRACTNORDRIVER_HPP
#define LIBSMART_STM32LEVELX_ABSTRACTNORDRIVER_HPP

#include <libsmart_config.hpp>
#include <main.h>
#include "lx_api.h"


namespace Stm32LevelX {
    class AbstractNorDriver {
    public:
        virtual ~AbstractNorDriver() = default;

        virtual ULONG getTotalBlocks() = 0;
        virtual ULONG getBlockSize() = 0;
        virtual UINT readSector(uint32_t addr, uint8_t *out, uint16_t size) = 0;
        virtual UINT writeSector(uint32_t addr, uint8_t *in, uint16_t size) = 0;
        // virtual UINT eraseBlock(ULONG block, ULONG erase_count) = 0;
        // virtual UINT eraseBlockVerify(ULONG block) = 0;

        virtual UINT reset() = 0;
    };
}
#endif
