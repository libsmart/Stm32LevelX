/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Sst26Driver.hpp"

using namespace Stm32LevelX::Driver;

ULONG Sst26Driver::getTotalBlocks() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getTotalBlocks()\r\n");

    return 512;
}

ULONG Sst26Driver::getBlockSize() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getBlockSize()\r\n");

    return 4096;
}

UINT Sst26Driver::readSector(const uint32_t addr, uint8_t *out, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::readSector(0x%08x, %p, %lu)\r\n",
                     addr, &out, size);
    const auto ret = READ(addr, out, size);
    return ret == Stm32Common::HalStatus::HAL_OK ? LX_SUCCESS : LX_ERROR;
}

UINT Sst26Driver::writeSector(uint32_t addr, uint8_t *in, uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::writeSector(0x%08x, %p, %lu)\r\n",
                     addr, &in, size);
    UNUSED(WREN());
    for (uint16_t i = 0; i <= ((size * sizeof(ULONG)) / 256); i++) {
        PP(addr + i * 256, &in[i * 256], std::min(size - i * 256, 256));
        if (waitForWriteFinish() == Stm32Common::HalStatus::HAL_TIMEOUT) {
            return LX_ERROR;
        }
    }
    UNUSED(WRDI());
    return LX_SUCCESS;
}

UINT Sst26Driver::reset() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::reset()\r\n");
    RSTEN();
    RST();
    return LX_SUCCESS;
}
