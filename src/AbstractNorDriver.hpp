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

        virtual ULONG getTotalSectors() = 0;

        virtual ULONG getSectorSize() = 0;

        virtual UINT read(uint32_t addr, uint8_t *out, uint16_t size) = 0;

        virtual UINT write(uint32_t addr, uint8_t *in, uint16_t size) = 0;

        virtual UINT eraseSector(uint32_t addr, ULONG erase_count) = 0;

        virtual UINT verifySectorErased(uint32_t addr) = 0;

        virtual UINT initialize() =0;

        virtual UINT reset() = 0;
    };
}
#endif
