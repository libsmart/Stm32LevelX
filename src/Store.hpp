/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_STORE_HPP
#define LIBSMART_STM32LEVELX_STORE_HPP

#include <main.h>
#include <libsmart_config.hpp>
#include "LevelXNorFlash.hpp"

namespace Stm32LevelX {
    template<class STORED_OBJECT>
    class Store : Stm32ItmLogger::Loggable {
    public:
        static constexpr uint32_t SECTOR_SIZE = LevelXNorFlash::getSectorSize();
        static constexpr uint32_t SECTORS = (sizeof(STORED_OBJECT) + SECTOR_SIZE - 1) / SECTOR_SIZE;

        /**
         * @brief Calculates the size of raw data required for storing a given object type in persistent storage.
         *
         * This method computes the size of the raw data buffer based on the size of the stored object
         * and the flash memory sector size. The calculation ensures that the raw data buffer is adequately
         * sized to contain the object, with sector alignment taken into account.
         *
         * @tparam STORED_OBJECT The type of the object to be stored, whose size determines the buffer requirements.
         * @return The size of the raw data buffer, expressed in the number of `ULONG` elements.
         */
        static constexpr size_t rawDataSize() {
            constexpr auto sectorSize = SECTOR_SIZE;
            return (sectorSize / sizeof(ULONG)) * ((sizeof(STORED_OBJECT) + sectorSize - 1) / sectorSize);
        }

        using rawData_t = ULONG[rawDataSize()];

        Store(LevelXNorFlash &lx, const uint32_t logicalSector, rawData_t &rawData)
            : Store(lx, logicalSector, rawData, nullptr) { ; }

        Store(LevelXNorFlash &lx, const uint32_t logicalSector, rawData_t &rawData,
              Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger), LX(lx), rawData(rawData),
              logicalSector(logicalSector) { initializeDefault(); }


        void initializeDefault() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::initializeDefault()\r\n");
            std::memset(rawData, LIBSMART_STM32LEVELX_STORE_INITIALIZE_BYTE, sizeof(rawData));
            data = new(rawData) STORED_OBJECT();
        }

        bool read() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::read()\r\n");

            if (!open()) {
                log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                        ->printf("Stm32LevelX::Store::open() failed\r\n");
                return false;
            }

            for (uint32_t i = 0; i < SECTORS; i++) {
                uint8_t *addr = reinterpret_cast<uint8_t *>(rawData) + i * SECTOR_SIZE;
                const auto ret = LX.sectorRead(logicalSector + i, addr);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                            ->printf("LX.sectorRead(%d, %p) = 0x%02x\r\n", logicalSector + i, rawData, ret);
                    return false;
                }
                log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->printf("LX.sectorRead(%d, %p) = 0x%02x\r\n", logicalSector + i, rawData, ret);
            }

            return true;
        }

        bool write() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::write()\r\n");

            open();

            for (uint32_t i = 0; i < SECTORS; i++) {
                uint8_t *addr = reinterpret_cast<uint8_t *>(rawData) + i * SECTOR_SIZE;
                const auto ret = LX.sectorWrite(logicalSector + i, addr);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                            ->printf("LX.sectorWrite(%d, %p) = 0x%02x\r\n", logicalSector + i, addr, ret);
                    return false;
                }
                log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::NOTICE)
                        ->printf("LX.sectorWrite(%d, %p) = 0x%02x\r\n", logicalSector + i, addr, ret);
            }

            return true;
        }


        bool release() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::release()\r\n");

            open();

            for (uint32_t i = 0; i < SECTORS; i++) {
                const auto ret = LX.sectorRelease(logicalSector + i);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                            ->printf("LX.sectorRelease(%d) = 0x%02x\r\n", logicalSector + i, ret);
                    return false;
                }
            }

            return true;
        }


        bool open() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Store::open()\r\n");

            bool ret = true;
            if (!LX.isInitialized()) {
                if (LX.initialize() != LevelXErrorCode::SUCCESS) {
                    ret = false;
                }
            }
            if (!LX.isOpen()) {
                if (LX.open() != LevelXErrorCode::SUCCESS) {
                    ret = false;
                }
            }
            return ret;
        }

        STORED_OBJECT *getStoredObject() { return data; }

        STORED_OBJECT &storedObject() {
            if (data == nullptr) {
                initializeDefault();
            }
            return *data;
        }

        void setLogicalSector(uint32_t logicalSector) { this->logicalSector = logicalSector; }

    private:
        LevelXNorFlash &LX;
        rawData_t &rawData;
        STORED_OBJECT *data = nullptr;
        uint32_t logicalSector;
    };
}
#endif
