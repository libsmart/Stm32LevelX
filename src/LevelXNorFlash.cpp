/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "LevelXNorFlash.hpp"

using namespace Stm32LevelX;

LevelXErrorCode LevelXNorFlash::initialize() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32LevelX::LevelXNorFlash::initialize()");

    LX_initialized = false;
    LX_open = false;

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_initialize
    auto ret = lx_nor_flash_initialize();
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_initialize() = 0x%02x\r\n", ret);
        return static_cast<LevelXErrorCode>(ret);
    }
    LX_initialized = true;
    return static_cast<LevelXErrorCode>(ret);
}


LevelXErrorCode LevelXNorFlash::open() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32LevelX::LevelXNorFlash::open()");

    if (!isInitialized()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_open(): NOT INITIALIZED\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_open
    auto ret = lx_nor_flash_open(this, const_cast<CHAR *>(getName()), driver_initialize);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_open() = 0x%02x\r\n", ret);
        return static_cast<LevelXErrorCode>(ret);
    }
    LX_open = true;
    return static_cast<LevelXErrorCode>(ret);
}

LevelXErrorCode LevelXNorFlash::close() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32LevelX::LevelXNorFlash::close()");

    if (!isOpen()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_close(): NOT OPEN\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_close
    auto ret = lx_nor_flash_close(this);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_close() = 0x%02x\r\n", ret);
        return static_cast<LevelXErrorCode>(ret);
    }
    LX_open = false;
    return static_cast<LevelXErrorCode>(ret);
}

LevelXErrorCode LevelXNorFlash::defragment() {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->println("Stm32LevelX::LevelXNorFlash::defragment()");

    if (!isOpen()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_defragment(): NOT OPEN\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_defragment
    auto ret = lx_nor_flash_defragment(this);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_defragment() = 0x%02x\r\n", ret);
        return static_cast<LevelXErrorCode>(ret);
    }
    LX_open = false;
    return static_cast<LevelXErrorCode>(ret);
}

LevelXErrorCode LevelXNorFlash::partialDefragment(const UINT max_blocks) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::LevelXNorFlash::partialDefragment(%d)\r\n", max_blocks);

    if (!isOpen()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_partial_defragment(): NOT OPEN\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_partial_defragment
    auto ret = lx_nor_flash_partial_defragment(this, max_blocks);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_partial_defragment() = 0x%02x\r\n", ret);
        return static_cast<LevelXErrorCode>(ret);
    }
    LX_open = false;
    return static_cast<LevelXErrorCode>(ret);
}

LevelXErrorCode LevelXNorFlash::sectorRead(const ULONG logical_sector, void *buffer) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::LevelXNorFlash::sectorRead(%lu)\r\n", logical_sector);

    if (!isOpen()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_sector_read(): NOT OPEN\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_sector_read
    auto ret = lx_nor_flash_sector_read(this, logical_sector, buffer);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_sector_read() = 0x%02x\r\n", ret);
    }
    return static_cast<LevelXErrorCode>(ret);
}

LevelXErrorCode LevelXNorFlash::sectorRelease(const ULONG logical_sector) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::LevelXNorFlash::sectorRelease(%lu)\r\n", logical_sector);

    if (!isOpen()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_sector_release(): NOT OPEN\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_sector_release
    auto ret = lx_nor_flash_sector_release(this, logical_sector);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_sector_release() = 0x%02x\r\n", ret);
    }
    return static_cast<LevelXErrorCode>(ret);
}

LevelXErrorCode LevelXNorFlash::sectorWrite(const ULONG logical_sector, void *buffer) {
    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
            ->printf("Stm32LevelX::LevelXNorFlash::sectorWrite(%lu)\r\n", logical_sector);

    if (!isOpen()) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_sector_write(): NOT OPEN\r\n");
        return LevelXErrorCode::ERROR;
    }

    // @see https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/levelx/chapter6.md#lx_nor_flash_sector_write
    auto ret = lx_nor_flash_sector_write(this, logical_sector, buffer);
    if (ret != LX_SUCCESS) {
        log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->printf("lx_nor_flash_sector_write() = 0x%02x\r\n", ret);
    }
    return static_cast<LevelXErrorCode>(ret);
}

