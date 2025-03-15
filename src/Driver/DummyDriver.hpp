/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <libsmart_config.hpp>
#include <main.h>

#include "../AbstractNorDriver.hpp"
#include "Loggable.hpp"

using namespace Stm32Common;

namespace Stm32LevelX::Driver {
    class DummyDriver : public AbstractNorDriver, public Stm32ItmLogger::Loggable {
    public:
        static constexpr uint32_t TOTAL_SECTORS = 512;
        static constexpr uint32_t SECTOR_SIZE = 4096;

        ULONG getTotalSectors() override;

        ULONG getSectorSize() override;

        UINT read(uint32_t addr, uint8_t *out, uint16_t size) override;

        UINT write(uint32_t addr, uint8_t *in, uint16_t size) override;

        UINT eraseSector(uint32_t addr, ULONG erase_count) override;

        UINT verifySectorErased(uint32_t addr) override;

        UINT initialize() override;

        UINT reset() override;
    };
}
