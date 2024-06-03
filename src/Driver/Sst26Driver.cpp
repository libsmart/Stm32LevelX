/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Sst26Driver.hpp"

using namespace Stm32LevelX::Driver;

ULONG Sst26Driver::getTotalSectors() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getTotalBlocks()\r\n");

    return 512;
}

ULONG Sst26Driver::getSectorSize() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getSectorSize()\r\n");

    return SECTOR_SIZE;
}

UINT Sst26Driver::read(const uint32_t addr, uint8_t *out, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::read(0x%08x, %p, %lu)\r\n",
                     addr, &out, size);
    const auto ret = READ(addr, out, size);
    return ret == HalStatus::HAL_OK ? LX_SUCCESS : LX_ERROR;
}

UINT Sst26Driver::write(const uint32_t addr, uint8_t *in, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::write(0x%08x, %p, %lu)\r\n",
                     addr, &in, size);

    const uint16_t SLICES = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint16_t i = 0; i < SLICES; i++) {
        const uint16_t sz = std::min(size - i * PAGE_SIZE, PAGE_SIZE);
        HalStatus ret = WREN();
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
        ret = PP(addr + i * PAGE_SIZE, &in[i * PAGE_SIZE], sz);
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
        ret = waitForWriteFinish(10);
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
    }
    WRDI();

    // Check written bytes
#ifndef LIBSMART_STM32LEVELX_PP_READ_BACK_TEST
    constexpr uint16_t BUFFER_SIZE = 32;
    uint8_t buffer[BUFFER_SIZE] = {};
    const uint16_t SLICES_CHK = (size + BUFFER_SIZE - 1) / BUFFER_SIZE;

    for (uint16_t iSlice = 0; iSlice < SLICES_CHK; iSlice++) {
        const uint16_t sz = std::min(static_cast<uint16_t>(size - iSlice * BUFFER_SIZE), BUFFER_SIZE);
        const HalStatus ret = READ(addr + iSlice * BUFFER_SIZE, buffer, sz);
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
        if (std::memcmp(buffer, &in[iSlice * BUFFER_SIZE], sz) != 0) return LX_INVALID_WRITE;
    }
#endif

    return LX_SUCCESS;
}

UINT Sst26Driver::eraseSector(const uint32_t addr, ULONG erase_count) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::eraseSector(0x%08x)\r\n",
                     addr);

    if (addr % SECTOR_SIZE > 0) return LX_ERROR;
    WREN();
    const HalStatus ret = SE(addr);
    WRDI();
    return ret == HalStatus::HAL_OK ? LX_SUCCESS : LX_ERROR;
}

UINT Sst26Driver::verifySectorErased(const uint32_t addr) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::verifySectorErased(0x%08x)\r\n",
                     addr);

    if (addr % SECTOR_SIZE > 0) return LX_ERROR;
    constexpr uint16_t BUFFER_SIZE = 32;
    constexpr uint16_t SLICES = (SECTOR_SIZE + BUFFER_SIZE - 1) / BUFFER_SIZE;
    uint8_t buffer[BUFFER_SIZE];
    std::memset(buffer, 0xFF, BUFFER_SIZE);

    for (uint16_t idx = 0; idx < SLICES; idx++) {
        const uint16_t sz = std::min(SECTOR_SIZE - idx * BUFFER_SIZE, static_cast<uint32_t>(BUFFER_SIZE));
        const HalStatus ret = READ(addr + idx * BUFFER_SIZE, buffer, sz);
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
        for (const uint8_t c: buffer) {
            if (c != 0xFF) return LX_ERROR;
        }
    }

    return LX_SUCCESS;
}

UINT Sst26Driver::initialize() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::initialize()\r\n");

    reset();
    if (waitForComOk(40) != HalStatus::HAL_OK) return LX_ERROR;
    WREN();
    ULBPR();
    WRDI();
    return LX_SUCCESS;
}

UINT Sst26Driver::reset() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::Driver::Sst26Driver::reset()\r\n");
    RSTEN();
    RST();
    return LX_SUCCESS;
}
