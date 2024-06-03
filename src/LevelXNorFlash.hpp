/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_LXNOR_HPP
#define LIBSMART_STM32LEVELX_LXNOR_HPP

#include <libsmart_config.hpp>
#include <main.h>

#include <AbstractNorDriver.hpp>
#include <Driver/Sst26Driver.hpp>

#include "Loggable.hpp"
#include "lx_api.h"
#include "Nameable.hpp"

// #define LX_NOR_SECTOR_SIZE 4096

namespace Stm32LevelX {
    static ULONG nor_sector_memory[LX_NOR_SECTOR_SIZE];


    enum class LevelXErrorCode : UINT {
        SUCCESS = 0x00,
        ERROR = 0x01,
        NO_SECTORS = 2,
        SECTOR_NOT_FOUND = 3,
        NO_PAGES = 4,
        INVALID_WRITE = 5,
        NAND_ERROR_CORRECTED = 6,
        NAND_ERROR_NOT_CORRECTED = 7,
        NO_MEMORY = 8,
        DISABLED = 9,
        SYSTEM_INVALID_FORMAT = 90,
        SYSTEM_INVALID_BLOCK = 91,
        SYSTEM_ALLOCATION_FAILED = 92,
        SYSTEM_MUTEX_CREATE_FAILED = 93,
        SYSTEM_INVALID_SECTOR_MAP = 94
    };

    class LevelXNorFlash : public Stm32ItmLogger::Loggable, public Stm32Common::Nameable, public LX_NOR_FLASH {
    public:
        explicit LevelXNorFlash(AbstractNorDriver *driver)
            : LX_NOR_FLASH_STRUCT(), driver(driver) { self = this; }

        LevelXNorFlash(AbstractNorDriver *driver, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger), LX_NOR_FLASH_STRUCT(), driver(driver) { self = this; }


        LevelXErrorCode initialize();

        LevelXErrorCode open();

        LevelXErrorCode close();

        LevelXErrorCode defragment();

        LevelXErrorCode partialDefragment(UINT max_blocks);

        LevelXErrorCode sectorRead(ULONG logical_sector, VOID *buffer);

        LevelXErrorCode sectorRelease(ULONG logical_sector);

        LevelXErrorCode sectorWrite(ULONG logical_sector, VOID *buffer);

        static constexpr uint32_t getSectorSize() { return LX_NOR_SECTOR_SIZE * sizeof(ULONG); }

        static const char *getErrorCodeString(const LevelXErrorCode errorCode) {
            switch (errorCode) {
                case LevelXErrorCode::SUCCESS:
                    return "SUCCESS";
                case LevelXErrorCode::ERROR:
                    return "ERROR";
                case LevelXErrorCode::NO_SECTORS:
                    return "NO_SECTORS";
                case LevelXErrorCode::SECTOR_NOT_FOUND:
                    return "SECTOR_NOT_FOUND";
                case LevelXErrorCode::NO_PAGES:
                    return "NO_PAGES";
                case LevelXErrorCode::INVALID_WRITE:
                    return "INVALID_WRITE";
                case LevelXErrorCode::NAND_ERROR_CORRECTED:
                    return "NAND_ERROR_CORRECTED";
                case LevelXErrorCode::NAND_ERROR_NOT_CORRECTED:
                    return "NAND_ERROR_NOT_CORRECTED";
                case LevelXErrorCode::NO_MEMORY:
                    return "NO_MEMORY";
                case LevelXErrorCode::DISABLED:
                    return "DISABLED";
                case LevelXErrorCode::SYSTEM_INVALID_FORMAT:
                    return "SYSTEM_INVALID_FORMAT";
                case LevelXErrorCode::SYSTEM_INVALID_BLOCK:
                    return "SYSTEM_INVALID_BLOCK";
                case LevelXErrorCode::SYSTEM_ALLOCATION_FAILED:
                    return "SYSTEM_ALLOCATION_FAILED";
                case LevelXErrorCode::SYSTEM_MUTEX_CREATE_FAILED:
                    return "SYSTEM_MUTEX_CREATE_FAILED";
                case LevelXErrorCode::SYSTEM_INVALID_SECTOR_MAP:
                    return "SYSTEM_INVALID_SECTOR_MAP";
            }
            return "UNKNOWN";
        }

        static UINT driver_initialize(LX_NOR_FLASH *nor_flash) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::LevelXNorFlash::driver_initialize()\r\n");


            self->driver->initialize();

            ULONG block_size = self->driver->getSectorSize();
            ULONG total_blocks = 512;

            /* Setup the base address of the flash memory.  */
            // nor_flash->lx_nor_flash_base_address = nullptr;

            /* Setup geometry of the flash.  */
            nor_flash->lx_nor_flash_total_blocks = total_blocks;
            nor_flash->lx_nor_flash_words_per_block = block_size / sizeof(ULONG);

            nor_flash->lx_nor_flash_driver_read = nor_driver_read;
            nor_flash->lx_nor_flash_driver_write = nor_driver_write;

            nor_flash->lx_nor_flash_driver_block_erase = nor_driver_block_erase;
            nor_flash->lx_nor_flash_driver_block_erased_verify = nor_driver_block_erased_verify;

            nor_flash->lx_nor_flash_driver_system_error = nor_driver_system_error;

            nor_flash->lx_nor_flash_sector_buffer = &nor_sector_memory[0];

            return LX_SUCCESS;
        }


        static UINT nor_driver_read(ULONG *flash_address, ULONG *destination, ULONG words) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::LevelXNorFlash::nor_driver_read_sector(0x%08x, 0x%08x, %d)\r\n",
                             flash_address, &destination, words);

            return self->driver->read(
                reinterpret_cast<uint32_t>(flash_address),
                reinterpret_cast<uint8_t *>(destination),
                words * sizeof(ULONG)
            );
        }

        static UINT nor_driver_write(ULONG *flash_address, ULONG *source, ULONG words) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::LevelXNorFlash::nor_driver_write_sector(0x%08x, 0x%08x, %d)\r\n",
                             flash_address, &source, words);

            return self->driver->write(
                reinterpret_cast<uint32_t>(flash_address),
                reinterpret_cast<uint8_t *>(source),
                words * sizeof(ULONG)
            );
        }


        static UINT nor_driver_block_erase(ULONG block, ULONG erase_count) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::LevelXNorFlash::nor_driver_block_erase(0x%08x, %d)\r\n",
                             block, erase_count);

            return self->driver->eraseSector(block * self->lx_nor_flash_words_per_block * sizeof(ULONG), erase_count);
        }

        static UINT nor_driver_block_erased_verify(ULONG block) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::LevelXNorFlash::nor_driver_block_erased_verify(0x%08x)\r\n",
                             block);

            return self->driver->verifySectorErased(block * self->lx_nor_flash_words_per_block * sizeof(ULONG));
        }

        static UINT nor_driver_system_error(UINT error_code) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                    ->printf("Stm32LevelX::LevelXNorFlash::nor_driver_system_error(0x%04x)\r\n",
                             error_code);

            // self->driver->reset();
            return self->driver->initialize();
            return LX_SUCCESS;
        }

    protected:
        AbstractNorDriver *driver;
        static LevelXNorFlash *self;
        bool isInitialized = false;
        bool isOpen = false;
    };

    inline LevelXNorFlash *LevelXNorFlash::self = {};
}

#endif
