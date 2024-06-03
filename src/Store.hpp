/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_STORE_HPP
#define LIBSMART_STM32LEVELX_STORE_HPP

namespace Stm32LevelX {
    template<class STORED_OBJECT>
    class Store : Stm32ItmLogger::Loggable {
    public:
        explicit Store(LevelXNorFlash *lx, const uint32_t logicalSector)
            : Store(lx, logicalSector, nullptr) { ; }

        Store(LevelXNorFlash *lx, const uint32_t logicalSector, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger),
              logicalSector(logicalSector),
              LX(lx) { initializeDefault(); }


        void initializeDefault() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::initializeDefault()\r\n");
            std::memset(rawData, LIBSMART_STM32LEVELX_STORE_INITIALIZE_BYTE, sizeof(rawData));
            data = new(rawData) STORED_OBJECT();
        }

        bool read() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::read()\r\n");

            open();

            for (uint32_t i = 0; i < SECTORS; i++) {
                uint8_t *addr = reinterpret_cast<uint8_t *>(rawData) + i * SECTOR_SIZE;
                const auto ret = LX->sectorRead(logicalSector + i, addr);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                            ->printf("LX->sectorRead(%d, %p) = 0x%02x\r\n", logicalSector, rawData, ret);
                    return false;
                }
                log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->printf("LX->sectorRead(%d, %p) = 0x%02x\r\n", logicalSector, rawData, ret);
            }

            return true;
        }

        bool write() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::write()\r\n");

            open();

            for (uint32_t i = 0; i < SECTORS; i++) {
                uint8_t *addr = reinterpret_cast<uint8_t *>(rawData) + i * SECTOR_SIZE;
                const auto ret = LX->sectorWrite(logicalSector + i, addr);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                            ->printf("LX->sectorWrite(%d, %p) = 0x%02x\r\n", logicalSector, addr, ret);
                    return false;
                }
                log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->printf("LX->sectorWrite(%d, %p) = 0x%02x\r\n", logicalSector, addr, ret);
            }

            return true;
        }


        bool release() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::release()\r\n");

            open();

            for (uint32_t i = 0; i < SECTORS; i++) {
                const auto ret = LX->sectorRelease(logicalSector + i);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                            ->printf("LX->sectorRelease(%d) = 0x%02x\r\n", logicalSector, ret);
                    return false;
                }
            }

            return true;
        }


        bool open() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::open()\r\n");

            bool ret = true;
            if (!LX->isInitialized()) {
                if (LX->initialize() != LevelXErrorCode::SUCCESS) {
                    ret = false;
                }
            }
            if (!LX->isOpen()) {
                if (LX->open() != LevelXErrorCode::SUCCESS) {
                    ret = false;
                }
            }
            return ret;
        }

        STORED_OBJECT *getStoredObject() { return data; }

    private:
        LevelXNorFlash *LX;
        uint32_t SECTOR_SIZE = LevelXNorFlash::getSectorSize();
        uint32_t SECTORS = (sizeof(STORED_OBJECT) + SECTOR_SIZE - 1) / SECTOR_SIZE;
        ULONG rawData[(LevelXNorFlash::getSectorSize() / sizeof(ULONG))
            * ((sizeof(STORED_OBJECT) + LevelXNorFlash::getSectorSize() - 1) / LevelXNorFlash::getSectorSize())
            ] = {};
        STORED_OBJECT *data = nullptr;
        uint32_t logicalSector;
    };
}
#endif
