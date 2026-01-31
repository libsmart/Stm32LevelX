/*
 * SPDX-FileCopyrightText: 2026 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <main.h>
#include <libsmart_config.hpp>
#include "Hash/MurmurHash3.hpp"
#include "LevelXNorFlash.hpp"

namespace Stm32LevelX {
    using namespace Stm32ItmLogger;

    template<class STORED_OBJECT>
    class Store : public Loggable {
    public:
        /** The logical sector size used in LevelX */
        static constexpr uint32_t SECTOR_SIZE = LevelXNorFlash::getSectorSize();
        /** Number of sectors required to store the <STORED_OBJECT> */
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

        /** Datatype for the data buffer used to store the <STORED_OBJECT> */
        using rawData_t = ULONG[rawDataSize()];

        Store(LevelXNorFlash &lx, const uint32_t logicalSector, rawData_t &rawData)
            : Store(lx, logicalSector, rawData, nullptr) { ; }

        Store(LevelXNorFlash &lx, const uint32_t logicalSector, rawData_t &rawData, LoggerInterface &logger)
            : Loggable(&logger), LX(lx), rawData(rawData), logicalSector(logicalSector) { initializeDefault(); }

        Store(LevelXNorFlash &lx, const uint32_t logicalSector, rawData_t &rawData, LoggerInterface *logger)
            : Loggable(logger), LX(lx), rawData(rawData), logicalSector(logicalSector) { initializeDefault(); }


        /**
         * @brief Initializes and retrieves a default instance of the stored object.
         *
         * This method initializes the storage buffer with a default byte value, constructs
         * a new instance of the stored object within the buffer, and returns a reference to it.
         * The logging mechanism is utilized to indicate the initialization process.
         *
         * @return A reference to the initialized default instance of the stored object.
         */
        STORED_OBJECT &initializeDefault() {
            log(LoggerInterface::Severity::DEBUGGING)
                    ->printf("Stm32LevelX::Store::initializeDefault()\r\n");
            std::memset(rawData, LIBSMART_STM32LEVELX_STORE_INITIALIZE_BYTE, sizeof(rawData));
            data = new(rawData) STORED_OBJECT();
            return *data;
        }

        /**
         * @brief Reads data from the underlying storage into the raw data buffer.
         *
         * This method attempts to read data from the storage across multiple sectors,
         * starting from the logical sector specified by the internal state. Each sector's
         * data is copied into the designated raw data buffer. Proper logging is performed
         * at various stages of the operation, indicating success or failure.
         *
         * If the storage cannot be opened or a read operation fails for any sector,
         * the method logs the error and terminates the operation prematurely.
         *
         * @return A boolean value indicating the success of the read operation.
         *         - `true`: All sectors were read successfully.
         *         - `false`: An error occurred while opening the storage or reading a sector.
         */
        bool read() const {
            log(LoggerInterface::Severity::DEBUGGING)->printf("Stm32LevelX::Store::read()\r\n");

            if (!open()) {
                log(LoggerInterface::Severity::ERROR)->printf("Stm32LevelX::Store::open() failed\r\n");
                return false;
            }

            for (uint32_t i = 0; i < SECTORS; i++) {
                uint8_t *addr = reinterpret_cast<uint8_t *>(rawData) + i * SECTOR_SIZE;
                const auto ret = LX.sectorRead(logicalSector + i, addr);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log(LoggerInterface::Severity::ERROR)
                            ->printf("LX.sectorRead(%d, %p) = 0x%02x\r\n", logicalSector + i, rawData, ret);
                    return false;
                }
                log(LoggerInterface::Severity::INFORMATIONAL)
                        ->printf("LX.sectorRead(%d, %p) = 0x%02x\r\n", logicalSector + i, rawData, ret);
            }

            return true;
        }

        /**
         * @brief Performs a write operation to persistent storage by dividing the data into sectors.
         *
         * This method writes the content of the raw data buffer into sequential logical sectors
         * in persistent storage. Each sector is processed individually and its status is logged.
         * In case a write operation for any sector fails, the method logs the error and stops the process,
         * returning `false`.
         *
         * @return `true` if all sectors are successfully written, otherwise `false` if an error occurs.
         */
        bool write() const {
            log(LoggerInterface::Severity::DEBUGGING)->printf("Stm32LevelX::Store::write()\r\n");

            if (!open()) return false;

            for (uint32_t i = 0; i < SECTORS; i++) {
                uint8_t *addr = reinterpret_cast<uint8_t *>(rawData) + i * SECTOR_SIZE;
                const auto ret = LX.sectorWrite(logicalSector + i, addr);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log(LoggerInterface::Severity::ERROR)
                            ->printf("LX.sectorWrite(%d, %p) = 0x%02x\r\n", logicalSector + i, addr, ret);
                    return false;
                }
                log(LoggerInterface::Severity::INFORMATIONAL)
                        ->printf("LX.sectorWrite(%d, %p) = 0x%02x\r\n", logicalSector + i, addr, ret);
            }

            return true;
        }


        /**
         * @brief Releases the allocated sectors in persistent storage.
         *
         * This method attempts to release all the sectors associated with the object in persistent storage.
         * It iterates through the allocated sectors and releases them one by one using the corresponding
         * release operation. If any sector fails to release, the process is aborted, and the method logs an error.
         *
         * @return True if all sectors are successfully released, otherwise false.
         */
        bool release() const {
            log(LoggerInterface::Severity::DEBUGGING)->printf("Stm32LevelX::Store::release()\r\n");

            if (!open()) return false;

            for (uint32_t i = 0; i < SECTORS; i++) {
                const auto ret = LX.sectorRelease(logicalSector + i);
                if (ret != LevelXErrorCode::SUCCESS) {
                    log(LoggerInterface::Severity::ERROR)
                            ->printf("LX.sectorRelease(%d) = 0x%02x\r\n", logicalSector + i, ret);
                    return false;
                }
                log(LoggerInterface::Severity::INFORMATIONAL)
                        ->printf("LX.sectorRelease(%d) = 0x%02x\r\n", logicalSector + i, ret);
            }

            return true;
        }


        /**
         * @brief Opens the LevelX storage interface, ensuring it is properly initialized and ready for use.
         *
         * This method verifies the initialization and open state of the LevelX storage interface. If the interface
         * is not initialized or open, it attempts to initialize and open it. The method logs informational messages
         * during the process and returns the result of these operations.
         *
         * @return True if the storage interface is successfully initialized and opened, false otherwise.
         */
        bool open() const {
            log(LoggerInterface::Severity::DEBUGGING)->printf("Stm32LevelX::Store::open()\r\n");

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

        /**
         * @brief Provides a pointer to the stored object.
         *
         * This method returns a pointer to the stored object within the storage system.
         * It ensures that the stored object can be accessed and manipulated externally
         * as needed.
         *
         * @return A pointer to the stored object of type `STORED_OBJECT`.
         */
        STORED_OBJECT *getStoredObject() { return &storedObject(); }

        /**
         * @brief Retrieves the stored object, initializing it to a default state if not already set.
         *
         * This method returns a reference to the stored object. If the data pointer is null, the method
         * initializes the stored object to its default state using `initializeDefault()` and returns the
         * reference to the newly created object. Otherwise, it returns the existing object.
         *
         * @return A reference to the stored object of type `STORED_OBJECT`.
         */
        STORED_OBJECT &storedObject() { return data == nullptr ? initializeDefault() : *data; }

        /**
         * @brief Sets the logical sector value for this storage object.
         *
         * This method assigns a logical sector number to the storage object.
         * The logical sector serves as an abstraction for organizing data
         * within the physical storage system, enabling efficient data management
         * and lookups.
         *
         * @param logical_sector The logical sector number to be assigned, represented as a 32-bit unsigned integer.
         */
        void setLogicalSector(const ULONG logical_sector) { this->logicalSector = logical_sector; }

        /**
         * @brief Retrieves the logical sector value associated with the store.
         *
         * This method returns the logical sector number used to identify
         * the storage region within the flash memory. Logical sectors are
         * an abstraction used for organizing data storage over physical
         * sectors.
         *
         * @return The logical sector number currently assigned, represented as a 32-bit unsigned integer.
         */
        ULONG getLogicalSector() const { return logicalSector; }

        /**
         * @brief Computes the hash value for the stored data using a predefined seed.
         *
         * This method calculates the hash of the stored object to verify data integrity
         * or detect changes. It uses a default hash seed predefined by the system
         * configuration.
         *
         * @return The computed hash value as a 32-bit unsigned integer.
         */
        [[nodiscard]] auto calcHash() const {
            return calcHash(LIBSMART_STM32LEVELX_MURMUR_HASH_SEED);
        }

        /**
         * @brief Computes the hash value for the stored raw data using the given seed.
         *
         * This method generates a 32-bit hash using the MurmurHash3 algorithm. The hash is
         * calculated over the stored raw data and can be used for validation or data integrity
         * checks. The seed value allows customization of the hashing process to produce
         * different results for the same input data if desired.
         *
         * @param seed The 32-bit unsigned integer seed value used for the hash computation.
         * @return The computed hash value as a 32-bit unsigned integer.
         */
        [[nodiscard]] auto calcHash(const uint32_t seed) const {
            return Hash::MurmurHash3::murmur3_32(reinterpret_cast<uint8_t *>(rawData), sizeof(rawData), seed);
        }

    private:
        LevelXNorFlash &LX;
        rawData_t &rawData;
        STORED_OBJECT *data = nullptr;
        ULONG logicalSector;
    };
}
