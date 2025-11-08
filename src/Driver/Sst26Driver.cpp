/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Sst26Driver.hpp"

using namespace Stm32LevelX::Driver;

HalStatus Sst26Driver::SFDP(const enum SFDP addr, uint8_t *pData, const uint16_t size) {
    return SFDP(static_cast<uint32_t>(addr), pData, size);
}

HalStatus Sst26Driver::NOP() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::NOP()\r\n");
    spi->select();
    const auto ret = spi->transmit(Instruction::NOP);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::RSTEN() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RSTEN()\r\n");
    spi->select();
    const auto ret = spi->transmit(Instruction::RSTEN);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::RST() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RST()\r\n");
    spi->select();
    const auto ret = spi->transmit(Instruction::RST);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::RDSR(uint8_t &statusRegister) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RDSR()\r\n");
    spi->select();
    auto ret = spi->transmit(Instruction::RDSR);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(&statusRegister, 1);
    spi->unselect();
    return ret;
}

uint8_t Sst26Driver::RDSR() {
    uint8_t statusRegister;
    RDSR(statusRegister);
    return statusRegister;
}

HalStatus Sst26Driver::waitForWriteFinish(const uint32_t timeout_ms) {
    const uint32_t start_ms = millis();
    while (isBUSY()) {
        delay(1);
        if ((timeout_ms > 0) && (millis() - start_ms > timeout_ms)) return HalStatus::HAL_TIMEOUT;
    }
    return HalStatus::HAL_OK;
}

HalStatus Sst26Driver::waitForWriteFinish() {
    return waitForWriteFinish(DEFAULT_TIMEOUT);
}

HalStatus Sst26Driver::WRSR(const uint8_t statusRegister, const uint8_t configurationRegister) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::WRSR(0x%02x, 0x%02x)\r\n",
                     statusRegister, configurationRegister);
    spi->select();
    auto ret = spi->transmit(Instruction::WRSR);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(statusRegister);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(configurationRegister);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::RDCR(uint8_t &configurationRegister) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RDCR()\r\n");
    spi->select();
    auto ret = spi->transmit(Instruction::RDCR);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(&configurationRegister, 1);
    spi->unselect();
    return ret;
}

uint8_t Sst26Driver::RDCR() {
    uint8_t configurationRegister;
    RDCR(configurationRegister);
    return configurationRegister;
}

HalStatus Sst26Driver::READ(const uint32_t addr, uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::READ(0x%08x, %p, %lu)\r\n",
                     addr, &pData, size);
    spi->select();
    auto ret = spi->transmit_be((Instruction::READ << 24) | (addr & 0x00FFFFFF));
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::READ_HS(const uint32_t addr, uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::READ_HS(0x%08x, %p, %lu)\r\n",
                     addr, &pData, size);
    spi->select();
    auto ret = spi->transmit(Instruction::READ_HS);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit_be(addr << 8 | 0xFF);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::WREN() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::WREN()\r\n");
    spi->select();
    auto ret = spi->transmit(Instruction::WREN);
    LIBSMART_UNUSED(ret);
    spi->unselect();
    return isWEL() ? HalStatus::HAL_OK : HalStatus::HAL_ERROR;
}

HalStatus Sst26Driver::WRDI() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::WRDI()\r\n");
    spi->select();
    auto ret = spi->transmit(Instruction::WRDI);
    LIBSMART_UNUSED(ret);
    spi->unselect();
    return !isWEL() ? HalStatus::HAL_OK : HalStatus::HAL_ERROR;
}

HalStatus Sst26Driver::SE(const uint32_t addr) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::SE(0x%08x)\r\n",
                     addr);

    if (!isWEL()) return HalStatus::HAL_ERROR;
    if (addr % SECTOR_SIZE > 0) return HalStatus::HAL_ERROR;
    spi->select();
    const auto ret = spi->transmit_be((Instruction::SE << 24) | (addr & 0x00FFFFFF));
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::CE() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::CE()\r\n");

    if (!isWEL()) return HalStatus::HAL_ERROR;
    spi->select();
    // Send the Erase Full Array command
    const auto ret = spi->transmit(Instruction::CE);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::PP(const uint32_t addr, uint8_t *in, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::PP(0x%08x, %p, %lu)\r\n",
                     addr, &in, size);
    if (!isWEL()) return HalStatus::HAL_ERROR;
    if (size > PAGE_SIZE) return HalStatus::HAL_ERROR;
    if (size <= 0) return HalStatus::HAL_ERROR;
    const auto sz = std::min(size, static_cast<uint16_t>(PAGE_SIZE - (addr & 0x000000FF)));
    spi->select();
    auto ret = spi->transmit_be(Instruction::PP << 24 | addr & 0x00FFFFFF);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(in, sz);
    spi->unselect();
    if (ret != HalStatus::HAL_OK) return ret;


    // Check written bytes
#ifdef LIBSMART_STM32LEVELX_PP_READ_BACK_TEST

    if (waitForWriteFinish() != HalStatus::HAL_OK) return HalStatus::HAL_TIMEOUT;
    constexpr uint16_t BUFFER_SIZE = 32;
    uint8_t buffer[BUFFER_SIZE] = {};
    const uint16_t SLICES = (size + BUFFER_SIZE - 1) / BUFFER_SIZE;

    for (uint16_t iSlice = 0; iSlice < SLICES; iSlice++) {
        const uint16_t sz = std::min(static_cast<uint16_t>(size - iSlice * BUFFER_SIZE), BUFFER_SIZE);
        const HalStatus ret = READ(addr + iSlice * BUFFER_SIZE, buffer, sz);
        if (ret != HalStatus::HAL_OK) return ret;
        if (std::memcmp(buffer, &in[iSlice * BUFFER_SIZE], sz) != 0) return HalStatus::HAL_ERROR;
    }

#endif

    return ret;
}

HalStatus Sst26Driver::RDID(uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RDID()\r\n");
    if (size < 3) return HalStatus::HAL_ERROR;
    spi->select();
    auto ret = spi->transmit(Instruction::RDID);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, 3);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::SFDP(const uint32_t addr, uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::SFDP(0x%08x, %p, %lu)\r\n",
                     addr, &pData, size);
    spi->select();
    auto ret = spi->transmit(Instruction::SFDP);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit_be(addr << 8 | 0xff);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
    spi->unselect();
    return ret;
}

uint8_t Sst26Driver::SFDP(const uint32_t addr) {
    uint8_t data = 0;
    SFDP(addr, &data, 1);
    return data;
}

uint8_t Sst26Driver::SFDP(const enum SFDP addr) {
    return SFDP(static_cast<uint32_t>(addr));
}

HalStatus Sst26Driver::RBPR(uint8_t *out, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RBPR()\r\n");
    spi->select();
    auto ret = spi->transmit(Instruction::RBPR);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(out, size);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::ULBPR() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::ULBPR()\r\n");
    spi->select();
    const auto ret = spi->transmit(Instruction::ULBPR);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::RSID(uint16_t addr, uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RSID(0x%08x, %p, %lu)\r\n",
                     addr, &pData, size);
    spi->select();
    auto ret = spi->transmit(Instruction::RSID);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit_be(addr);
    ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(0xff);
    ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::DPD() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::DPD()\r\n");
    spi->select();
    const auto ret = spi->transmit(Instruction::DPD);
    spi->unselect();
    return ret;
}

HalStatus Sst26Driver::RDPD() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::RDPD()\r\n");
    spi->select();
    const auto ret = spi->transmit(Instruction::RDPD);
    spi->unselect();
    return ret;
}

bool Sst26Driver::isComOk() {
    // log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
    //         ->printf("Stm32LevelX::Driver::Sst26Driver::isComOk()\r\n");
    uint8_t jedecId[3] = {};
    const auto ret = RDID(jedecId, sizeof(jedecId));
    return (ret == HalStatus::HAL_OK)
           && (jedecId[0] == JEDECID::BYTE_0)
           && (jedecId[1] == JEDECID::BYTE_1)
           && (jedecId[2] == JEDECID::BYTE_2);
}

HalStatus Sst26Driver::waitForComOk(const uint32_t timeout_ms) {
    const uint32_t start_ms = millis();
    while (!isComOk()) {
        delay(1);
        if ((timeout_ms > 0) && (millis() - start_ms > timeout_ms)) return HalStatus::HAL_TIMEOUT;
    }
    return HalStatus::HAL_OK;
}

HalStatus Sst26Driver::waitForComOk() {
    return waitForComOk(DEFAULT_TIMEOUT);
}

HalStatus Sst26Driver::getEUI48(uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getEUI48()\r\n");
    if (size < 6) return HalStatus::HAL_ERROR;
    if (SFDP(SFDP::EUI48_PROGRAMMED) == 0x30) {
        return SFDP(0x261, pData, 6);
    }
    return HalStatus::HAL_ERROR;
}

HalStatus Sst26Driver::getEUI64(uint8_t *pData, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getEUI64()\r\n");
    if (size < 8) return HalStatus::HAL_ERROR;
    if (SFDP(SFDP::EUI64_PROGRAMMED) == 0x40) {
        return SFDP(0x268, pData, 8);
    }
    return HalStatus::HAL_ERROR;
}

ULONG Sst26Driver::getTotalSectors() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getTotalBlocks()\r\n");

    constexpr ULONG totalFlashSizeInMBit = 16;
    constexpr ULONG totalFlashSize = (totalFlashSizeInMBit * 1024 * 1024) / 8;

    return totalFlashSize / getSectorSize();
}

ULONG Sst26Driver::getSectorSize() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::getSectorSize()\r\n");

    return SECTOR_SIZE;
}

UINT Sst26Driver::read(const uint32_t addr, uint8_t *out, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::read(0x%08x, %p, %lu)\r\n",
                     addr, &out, size);
    const auto ret = READ(addr, out, size);
    return ret == HalStatus::HAL_OK ? LX_SUCCESS : LX_ERROR;
}

UINT Sst26Driver::write(const uint32_t addr, uint8_t *in, const uint16_t size) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::write(0x%08x, %p, %lu)\r\n",
                     addr, &in, size);

    const uint16_t SLICES = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint16_t i = 0; i < SLICES; i++) {
        const uint16_t sz = std::min(size - i * PAGE_SIZE, PAGE_SIZE);
        HalStatus ret = WREN();
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
        ret = PP(addr + i * PAGE_SIZE, &in[i * PAGE_SIZE], sz);
        if (ret != HalStatus::HAL_OK) return LX_ERROR;
        ret = waitForWriteFinish();
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
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::eraseSector(0x%08x)\r\n",
                     addr);

    if (addr % SECTOR_SIZE > 0) return LX_ERROR;
    WREN();
    HalStatus ret = SE(addr);
    ret = ret != HalStatus::HAL_OK ? ret : waitForWriteFinish();
    WRDI();
    return ret == HalStatus::HAL_OK ? LX_SUCCESS : LX_ERROR;
}

UINT Sst26Driver::verifySectorErased(const uint32_t addr) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::verifySectorErased(0x%08x)\r\n",
                     addr);

    if (addr % SECTOR_SIZE > 0) return LX_ERROR;
    constexpr uint16_t BUFFER_SIZE = PAGE_SIZE;
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
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::initialize()\r\n");

    reset();
    if (waitForComOk(std::max(40UL, DEFAULT_TIMEOUT)) != HalStatus::HAL_OK) return LX_ERROR;
    WREN();
    ULBPR();
    WRDI();
    return LX_SUCCESS;
}

UINT Sst26Driver::reset() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::DEBUGGING)
            ->printf("Stm32LevelX::Driver::Sst26Driver::reset()\r\n");
    RSTEN();
    RST();
    return LX_SUCCESS;
}

UINT Sst26Driver::chipErase() {
    auto ret = WREN();
    ret = ret != HalStatus::HAL_OK ? ret : CE();
    // Wait at least 50ms (data sheet)
    ret = ret != HalStatus::HAL_OK ? ret : waitForWriteFinish(std::max(50UL, DEFAULT_TIMEOUT));
    ret = ret != HalStatus::HAL_OK ? ret : WRDI();

    return ret == HalStatus::HAL_OK ? LX_SUCCESS : LX_ERROR;
}
