/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_DRIVER_SST26DRIVER_HPP
#define LIBSMART_STM32LEVELX_DRIVER_SST26DRIVER_HPP

#include <libsmart_config.hpp>
#include <main.h>

#include "../AbstractNorDriver.hpp"
#include "Stm32Spi.hpp"


namespace Stm32LevelX::Driver {
    class Sst26Driver : public AbstractNorDriver, public Stm32ItmLogger::Loggable {
    public:
        explicit Sst26Driver(Stm32Spi::Spi *spi)
            : spi(spi) { ; }

        Sst26Driver(Stm32Spi::Spi *spi, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger),
              spi(spi) { ; }


        class Instruction {
        public:
            static constexpr uint8_t NOP = 0x00; ///< No Operation
            static constexpr uint8_t RSTEN = 0x66; ///< Reset Enable
            static constexpr uint8_t RST = 0x99; ///< Reset Memory
            static constexpr uint8_t EQIO = 0x38; ///< Enable Quad I/O (NOT IMPLEMENTED)
            static constexpr uint8_t RSTQIO = 0xFF; ///< Reset Quad I/O (NOT IMPLEMENTED)
            static constexpr uint8_t RDSR = 0x05; ///< Read Status Register
            static constexpr uint8_t WRSR = 0x01; ///< Write Status Register
            static constexpr uint8_t RDCR = 0x35; ///< Read Configuration Register

            static constexpr uint8_t READ = 0x03; ///< Read Memory
            static constexpr uint8_t READ_HS = 0x0B; ///< Read Memory at Higher Speed
            static constexpr uint8_t SQOR = 0x6B; ///< SPI Quad Output Read
            static constexpr uint8_t SQIOR = 0xEB; ///< SPI Quad I/O Read
            static constexpr uint8_t SDOR = 0x3B; ///< SPI Dual Output Read
            static constexpr uint8_t SDIOR = 0xBB; ///< SPI Dual I/O Read
            static constexpr uint8_t SB = 0xC0; ///< Set Burst Length
            static constexpr uint8_t RBSQI = 0x0C; ///< SQI Read Burst with Wrap
            static constexpr uint8_t RBSPI = 0xEC; ///< SPI Read Burst with Wrap

            static constexpr uint8_t RDID = 0x9F; ///< JEDEC-ID Read
            static constexpr uint8_t SQRDID = 0xAF; ///< Quad I/O J-ID Read
            static constexpr uint8_t SFDP = 0x5A; ///< JEDEC-ID Read

            static constexpr uint8_t WREN = 0x06; ///< Write Enable
            static constexpr uint8_t WRDI = 0x04; ///< Write Disable
            static constexpr uint8_t SE = 0x20; ///< Erase 4 KBytes of Memory Array
            static constexpr uint8_t BE = 0xD8; ///< Erase 64, 32 or 8 KBytes of Memory Array
            static constexpr uint8_t CE = 0xC7; ///< Erase Full Array
            static constexpr uint8_t PP = 0x02; ///< Page Program
            static constexpr uint8_t SQPP = 0x32; ///< SQI Quad Page Program
            static constexpr uint8_t WRSU = 0xB0; ///< Suspends Program/Erase
            static constexpr uint8_t WRRE = 0x30; ///< Resumes Program/Erase

            static constexpr uint8_t RBPR = 0x72; ///< Read Block-Protection Register
            static constexpr uint8_t WBPR = 0x42; ///< Write Block-Protection Register
            static constexpr uint8_t LBPR = 0x8D; ///< Lock Down Block-Protection Register
            static constexpr uint8_t nVWLDR = 0xE8; ///< non-Volatile Write Lock-Down Register
            static constexpr uint8_t ULBPR = 0x98; ///< Global Block Protection Unlock
            static constexpr uint8_t RSID = 0x88; ///< Read Security ID
            static constexpr uint8_t PSID = 0xA5; ///< Program User Security ID area
            static constexpr uint8_t LSID = 0x85; ///< Lockout Security ID Programming

            static constexpr uint8_t DPD = 0xB9; ///< Deep Power-down Mode
            static constexpr uint8_t RDPD = 0xAB; ///< Release from Deep Power-down and Read ID
        };

        enum class SFDP : uint32_t {
            EUI48_PROGRAMMED = 0x260,
            EUI48_OCTET5 = 0x261,
            EUI48_OCTET4 = 0x262,
            EUI48_OCTET3 = 0x263,
            EUI48_OCTET2 = 0x264,
            EUI48_OCTET1 = 0x265,
            EUI48_OCTET0 = 0x266,

            EUI64_PROGRAMMED = 0x267,
            EUI64_OCTET7 = 0x268,
            EUI64_OCTET6 = 0x269,
            EUI64_OCTET5 = 0x26A,
            EUI64_OCTET4 = 0x26B,
            EUI64_OCTET3 = 0x26C,
            EUI64_OCTET2 = 0x26D,
            EUI64_OCTET1 = 0x26E,
            EUI64_OCTET0 = 0x26F,
        };


        /**
         * @brief Reads the manufacturer ID and device ID from the device.
         *
         * @param pData Pointer to the buffer where the ID data will be stored.
         * @param size Size of the buffer.
         * @return A `HalStatus` value representing the status of the operation.
         */
        Stm32Common::HalStatus RDID(uint8_t *pData, const uint16_t size) const {
            if (size < 3) return Stm32Common::HalStatus::HAL_ERROR;
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::RDID);
            ret = ret != Stm32Common::HalStatus::HAL_OK ? ret : spi->receive(pData, 3);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Sends the SFDP (Serial Flash Discoverable Parameters) instruction to the device.
         *
         * This method is used to retrieve the internal parameters of the flash memory device, such as memory capacity,
         * erase sizes, and program times. The address and size parameters determine the portion of the memory that will be read.
         *
         * @param addr The starting address in the memory to read from.
         * @param pData Pointer to the buffer where the data will be stored.
         * @param size The number of bytes to read.
         * @return A `HalStatus` value representing the status of the operation.
         */
        Stm32Common::HalStatus SFDP(const uint32_t addr, uint8_t *pData, const uint16_t size) const {
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::SFDP);
            ret = ret != Stm32Common::HalStatus::HAL_OK ? ret : spi->transmit_be(addr << 8 | 0xff);
            ret = ret != Stm32Common::HalStatus::HAL_OK ? ret : spi->receive(pData, size);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Sends the SFDP (Serial Flash Discoverable Parameters) instruction to the device.
         *
         * This method is used to retrieve the internal parameters of the flash memory device, such as memory capacity,
         * erase sizes, and program times. The address and size parameters determine the portion of the memory that will be read.
         *
         * @param addr The starting address in the memory to read from.
         * @param pData Pointer to the buffer where the data will be stored.
         * @param size The number of bytes to read.
         * @return A `HalStatus` value representing the status of the operation.
         */
        Stm32Common::HalStatus SFDP(const enum SFDP addr, uint8_t *pData, const uint16_t size) const {
            return SFDP(static_cast<uint32_t>(addr), pData, size);
        }

        /**
         * @brief Sends the SFDP (Serial Flash Discoverable Parameters) instruction to the device.
         *
         * This method is used to retrieve the internal parameters of the flash memory device, such as memory capacity,
         * erase sizes, and program times. The address parameter determines the starting address in the memory to read from.
         *
         * @param addr The starting address in the memory to read from.
         * @return The data read from the memory at the specified address.
         * @note This method reads a single byte of data from the memory.
         */
        [[nodiscard]] uint8_t SFDP(const uint32_t addr) const {
            uint8_t data = 0;
            SFDP(addr, &data, 1);
            return data;
        }


        /**
         * @brief Retrieves the value of the Serial Flash Discoverable Parameters (SFDP) register.
         *
         * This method returns the value of the SFDP register for the specified address.
         *
         * @param addr The address of the SFDP register to be read.
         * @return The value of the SFDP register.
         */
        [[nodiscard]] uint8_t SFDP(const enum SFDP addr) const {
            return SFDP(static_cast<uint32_t>(addr));
        }


        /**
         * @brief Reads data from the security ID space.
         *
         * @param addr The starting address in the security ID space to read from.
         * @param pData Pointer to the buffer where the read data will be stored.
         * @param size The number of bytes to read.
         * @return A `HalStatus` value representing the status of the operation.
         */
        Stm32Common::HalStatus RSID(uint16_t addr, uint8_t *pData, const uint16_t size) const {
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::RSID);
            ret = ret != Stm32Common::HalStatus::HAL_OK ? ret : spi->transmit_be(addr);
            ret = ret != Stm32Common::HalStatus::HAL_OK ? ret : spi->transmit(0xff);
            ret = ret != Stm32Common::HalStatus::HAL_OK ? ret : spi->receive(pData, size);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Retrieves the EUI-48 (Extended Unique Identifier) from the device.
         *
         * This method retrieves the EUI-48 from the device and stores it in the provided buffer.
         * The EUI-48 is a globally unique identifier assigned to a device's network interface.
         *
         * @param pData Pointer to the buffer where the EUI-48 will be stored.
         * @param size The size of the buffer.
         * @return A `HalStatus` value representing the status of the operation:
         */
        Stm32Common::HalStatus getEUI48(uint8_t *pData, const uint16_t size) const {
            if (size < 6) return Stm32Common::HalStatus::HAL_ERROR;
            memset(pData, 0, size);
            if (SFDP(SFDP::EUI48_PROGRAMMED) == 0x30) {
                return SFDP(0x261, pData, 6);
            }
            return Stm32Common::HalStatus::HAL_ERROR;
        }


        /**
         * @brief Retrieves the EUI-64 (Extended Unique Identifier) from the device.
         *
         * This method retrieves the EUI-64 from the device and stores it in the provided buffer.
         * The EUI-64 is a globally unique identifier assigned to a device's network interface.
         *
         * @param pData Pointer to the buffer where the EUI-64 will be stored. The buffer should have a size of at least 8 bytes.
         * @param size The size of the buffer. This should be at least 8.
         * @return A `HalStatus` value representing the status of the operation:
         */
        Stm32Common::HalStatus getEUI64(uint8_t *pData, const uint16_t size) const {
            if (size < 8) return Stm32Common::HalStatus::HAL_ERROR;
            memset(pData, 0, size);
            if (SFDP(SFDP::EUI64_PROGRAMMED) == 0x40) {
                return SFDP(0x268, pData, 8);
            }
            return Stm32Common::HalStatus::HAL_ERROR;
        }

    protected:
        Stm32Spi::Spi *spi;
    };
}

#endif
