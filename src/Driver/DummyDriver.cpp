/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DummyDriver.hpp"

ULONG Stm32LevelX::Driver::DummyDriver::getTotalSectors() {
    return TOTAL_SECTORS;
}

ULONG Stm32LevelX::Driver::DummyDriver::getSectorSize() {
    return SECTOR_SIZE;
}

UINT Stm32LevelX::Driver::DummyDriver::read(uint32_t addr, uint8_t *out, uint16_t size) {
    memset(out, 0xff, size);
    return LX_SUCCESS;
}

UINT Stm32LevelX::Driver::DummyDriver::write(uint32_t addr, uint8_t *in, uint16_t size) {
    return LX_SUCCESS;
}

UINT Stm32LevelX::Driver::DummyDriver::eraseSector(uint32_t addr, ULONG erase_count) {
    return LX_SUCCESS;
}

UINT Stm32LevelX::Driver::DummyDriver::verifySectorErased(uint32_t addr) {
    return LX_SUCCESS;
}

UINT Stm32LevelX::Driver::DummyDriver::initialize() {
    return LX_SUCCESS;
}

UINT Stm32LevelX::Driver::DummyDriver::reset() {
    return LX_SUCCESS;
}

UINT Stm32LevelX::Driver::DummyDriver::chipErase() {
    return LX_SUCCESS;
}
