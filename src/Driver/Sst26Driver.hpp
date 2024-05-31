/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_DRIVER_SST26DRIVER_HPP
#define LIBSMART_STM32LEVELX_DRIVER_SST26DRIVER_HPP

#include <ctime>
#include <libsmart_config.hpp>
#include <main.h>

#include "../AbstractNorDriver.hpp"
#include "Stm32Spi.hpp"

using namespace Stm32Common;

namespace Stm32LevelX::Driver {
    /**
     * @brief Driver for the SST26 flash device.
     * @see https://www.microchip.com/en-us/product/sst26vf016beui
     */
    class Sst26Driver : public AbstractNorDriver, public Stm32ItmLogger::Loggable {
    public:
        explicit Sst26Driver(Stm32Spi::Spi *spi)
            : spi(spi) { ; }

        Sst26Driver(Stm32Spi::Spi *spi, Stm32ItmLogger::LoggerInterface *logger)
            : Loggable(logger),
              spi(spi) { ; }


        static constexpr uint32_t PAGE_SIZE = 256;

        class JEDECID {
        public:
            static constexpr uint8_t BYTE_0 = 0xBF;
            static constexpr uint8_t MANUFACTURER_ID = BYTE_0;
            static constexpr uint8_t BYTE_1 = 0x26;
            static constexpr uint8_t DEVICE_TYPE = BYTE_1;
            static constexpr uint8_t BYTE_2 = 0x41;
            static constexpr uint8_t DEVICE_ID = BYTE_2;
        };

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
            SECTOR_TYPE_1_SIZE = 0x04C,

            SECTOR_MAP = 0x100,
            SECTOR_CONFIGURATION_ID = 0x101,
            SECTOR_REGION_COUNT = 0x102,
            SECTOR_RESERVED = 0x103,
            SECTOR_REGION_0_ERASE_SUPPORT = 0x104,
            SECTOR_REGION_0_SIZE = 0x105,


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
         * @brief No Operation.
         *
         * @return The result of the transmit operation.
         */
        HalStatus NOP() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::NOP()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::NOP);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Reset enable command for the Sst26Driver.
         *
         * @returns The return value of the spi->transmit() method, which is the result of the RSTEN command.
         */
        HalStatus RSTEN() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RSTEN()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::RSTEN);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Resets the Sst26Driver device.
         *
         * @return The status of the operation.
         */
        HalStatus RST() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RST()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::RST);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Read the status register of the SST26 flash driver.
         *
         * This function reads the status register of the SST26 flash driver. It sets the severity of the logger to
         * informational and prints a debug message indicating the entry into the RDSR function. It then selects the SPI,
         * transmits the RDSR instruction to the flash driver, and receives the status register value from the flash driver.
         * Finally, it unselects the SPI and returns the status of the operation.
         *
         * @param[in,out] statusRegister A reference to a variable where the status register value will be stored.
         *
         * @return The HAL status indicating the success or failure of the operation.
         */
        HalStatus RDSR(uint8_t &statusRegister) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RDSR()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::RDSR);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(&statusRegister, 1);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Read the status register of the device.
         *
         * This function reads the status register of the device and returns its value.
         *
         * @return The value of the status register.
         */
        [[nodiscard]] uint8_t RDSR() {
            uint8_t statusRegister;
            RDSR(statusRegister);
            return statusRegister;
        }

        [[nodiscard]] bool isBUSY() { return (RDSR() & 1 << 0) > 0; }
        [[nodiscard]] bool isWEL() { return (RDSR() & 1 << 1) > 0; }
        [[nodiscard]] bool isWSE() { return (RDSR() & 1 << 2) > 0; }
        [[nodiscard]] bool isWSP() { return (RDSR() & 1 << 3) > 0; }
        [[nodiscard]] bool isWPLD() { return (RDSR() & 1 << 4) > 0; }
        [[nodiscard]] bool isSEC() { return (RDSR() & 1 << 5) > 0; }


        /**
         * @brief Waits for the write operation to finish within the specified timeout period.
         *
         * This function waits for the write operation to finish by repeatedly checking the BUSY status.
         * It delays for 1 millisecond between each check. If the timeout period is specified and exceeded,
         * the function will return HAL_TIMEOUT status.
         *
         * @param timeout_ms The timeout period in milliseconds.
         *                   Set to 0 to wait indefinitely until the write operation finishes.
         *
         * @return The HAL status indicating the result of the write operation.
         *         - HAL_OK: The write operation finished successfully.
         *         - HAL_TIMEOUT: The timeout period was exceeded before the write operation finished.
         */
        HalStatus waitForWriteFinish(const uint32_t timeout_ms) {
            const uint32_t start_ms = millis();
            while (isBUSY()) {
                delay(1);
                if ((timeout_ms > 0) && (millis() - start_ms > timeout_ms)) return HalStatus::HAL_TIMEOUT;
            }
            return HalStatus::HAL_OK;
        }


        /**
         * @brief Waits for the write operation to finish.
         *
         * @return The HAL status indicating the result of the write operation.
         */
        HalStatus waitForWriteFinish() {
            return waitForWriteFinish(0);
        }


        /**
         * @brief Write Status Register (WRSR) method
         *
         * This method is used to write the status register and configuration register of the SST26 flash memory.
         *
         * @param statusRegister The value to be written to the status register. Must be a uint8_t.
         * @param configurationRegister The value to be written to the configuration register. Must be a uint8_t.
         *
         * @return The status of the operation. The return type is HalStatus and can be one of the following values:
         *         - HalStatus::HAL_OK: Operation successful.
         *         - Other HalStatus values: Operation unsuccessful, refer to the HalStatus documentation for detailed error codes.
         */
        HalStatus WRSR(const uint8_t statusRegister, const uint8_t configurationRegister) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::WRSR(0x%02x, 0x%02x)\r\n",
                             statusRegister, configurationRegister);
            spi->select();
            auto ret = spi->transmit(Instruction::WRSR);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(statusRegister);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(configurationRegister);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Read the Configuration Register (RDCR) of the SST26 flash memory.
         *
         * This function reads the Configuration Register (CR) of the SST26 flash memory and stores the value in the
         * provided configurationRegister variable.
         *
         * @param configurationRegister The variable to store the Configuration Register (CR) value.
         *
         * @return The status of the operation. Returns HalStatus::HAL_OK if successful, else an error status.
         */
        HalStatus RDCR(uint8_t &configurationRegister) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RDCR()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::RDCR);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(&configurationRegister, 1);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Reads the Configuration Register (RDCR).
         *
         * This function reads the Configuration Register (RDCR) and returns its value.
         *
         * @return The value of the Configuration Register.
         */
        [[nodiscard]] uint8_t RDCR() {
            uint8_t configurationRegister;
            RDCR(configurationRegister);
            return configurationRegister;
        }

        [[nodiscard]] bool isIOC() { return (RDCR() & 1 << 1) > 0; }
        [[nodiscard]] bool isBPNV() { return (RDCR() & 1 << 3) > 0; }
        [[nodiscard]] bool isWPEN() { return (RDCR() & 1 << 7) > 0; }


        /**
         * @brief Read data from a specific address.
         *
         * This method is used to read data from a specific address. It selects the SPI, clears the data buffer,
         * transmits the read instruction and address, and receives the data if the transmission is successful. It
         * finally unselects the SPI and returns the status of the operation.
         *
         * @param addr The address from which to read the data.
         * @param pData Pointer to the buffer where the read data will be stored.
         * @param size The number of bytes to read.
         *
         * @return The status of the operation (HalStatus::HAL_OK if successful, an error code otherwise).
         */
        HalStatus READ(const uint32_t addr, uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::READ(0x%08x, %p, %lu)\r\n",
                             addr, &pData, size);
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit_be((Instruction::READ << 24) | (addr & 0x00FFFFFF));
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Read data from a specific address using High Speed SPI interface.
         *
         * This function reads data from a specific address using the High Speed SPI
         * interface. It first selects the SPI interface, fills the data buffer with
         * zeros, transmits the READ_HS instruction, transmits the address shifted
         * left 8 bits ORed with 0xFF, receives the data, and finally unselects the
         * SPI interface.
         *
         * @param addr The address to read from.
         * @param pData A pointer to the buffer where the read data will be stored.
         * @param size The size of the data to read, in bytes.
         *
         * @return The status of the read operation. Possible values are:
         *         - HalStatus::HAL_OK: If the read operation is successful.
         *         - Other values: If the read operation fails.
         */
        HalStatus READ_HS(const uint32_t addr, uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::READ_HS(0x%08x, %p, %lu)\r\n",
                             addr, &pData, size);
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::READ_HS);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit_be(addr << 8 | 0xFF);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Write Enable (WREN)
         *
         * This method enables write operations to the SST26 memory chip.
         * The Write-Enable Latch bit is automatically reset under
         * the following conditions:
         * - Power-up
         * - Reset
         * - Write-Disable (WRDI) instruction
         * - Page-Program instruction completion
         * - Sector-Erase instruction completion
         * - Block-Erase instruction completion
         * - Chip-Erase instruction completion
         * - Write-Block-Protection register instruction
         * - Lock-Down Block-Protection register instruction
         * - Program Security ID instruction completion
         * - Lockout Security ID instruction completion
         * - Write-Suspend instruction
         * - SPI Quad Page program instruction completion
         * - Write Status Register
         *
         * @return The HAL status indicating the success or failure of the write enable operation. Possible values are:
         *         - `HalStatus::HAL_OK` if the write enable operation was successful
         *         - `HalStatus::HAL_ERROR` if the write enable operation failed
         */
        HalStatus WREN() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::WREN()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::WREN);
            spi->unselect();
            return isWEL() ? HalStatus::HAL_OK : HalStatus::HAL_ERROR;
        }


        /**
         * @brief Clear the Write Enable Latch (WEL) bit in the Sst26Driver.
         *
         * This method clears the Write Enable Latch (WEL) bit in the Sst26Driver.
         *
         * @return HalStatus::HAL_OK if the Write Enable Latch (WEL) bit is successfully cleared, HalStatus::HAL_ERROR otherwise.
         */
        HalStatus WRDI() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::WRDI()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::WRDI);
            spi->unselect();
            return !isWEL() ? HalStatus::HAL_OK : HalStatus::HAL_ERROR;
        }


        /**
         * @brief Program a page (256 bytes) of data to the specified address.
         *
         * This function programs a page of data to the specified address in the memory.
         *
         * @param addr The address to program the data to.
         * @param pData A pointer to the data to program.
         * @param size The size of the data to program in bytes.
         *
         * @return A HalStatus value indicating the success or failure of the operation.
         *         - HAL_OK: Operation was successful.
         *         - HAL_ERROR: WEL flag is not set or size is greater than 256.
         */
        HalStatus PP(const uint32_t addr, uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::PP(0x%08x, %p, %lu)\r\n",
                             addr, &pData, size);
            if (!isWEL()) return HalStatus::HAL_ERROR;
            if (size > PAGE_SIZE) return HalStatus::HAL_ERROR;
            if (size <= 0) return HalStatus::HAL_ERROR;
            const auto sz = std::min(size, static_cast<uint16_t>(PAGE_SIZE - (addr & 0x000000FF)));
            spi->select();
            auto ret = spi->transmit_be(Instruction::PP << 24 | addr & 0x00FFFFFF);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(pData, sz);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Reads the manufacturer ID and device ID from the device.
         *
         * @param pData Pointer to the buffer where the ID data will be stored.
         * @param size Size of the buffer.
         * @return A `HalStatus` value representing the status of the operation.
         */
        HalStatus RDID(uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RDID()\r\n");
            if (size < 3) return HalStatus::HAL_ERROR;
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::RDID);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, 3);
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
        HalStatus SFDP(const uint32_t addr, uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::SFDP(0x%08x, %p, %lu)\r\n",
                             addr, &pData, size);
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::SFDP);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit_be(addr << 8 | 0xff);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
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
        HalStatus SFDP(const enum SFDP addr, uint8_t *pData, const uint16_t size) {
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
        [[nodiscard]] uint8_t SFDP(const uint32_t addr) {
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
        [[nodiscard]] uint8_t SFDP(const enum SFDP addr) {
            return SFDP(static_cast<uint32_t>(addr));
        }


        /**
         * @brief Reads the current Block Protection Register (BPR) value of the SST26 flash device.
         *
         * This method sends the Read Block Protection Register (RBPR) instruction to the flash device
         * and reads the corresponding BPR value. It also logs the operation details using the ITM logger.
         *
         * @return The current Block Protection Register (BPR) value.
         */
        HalStatus RBPR(uint8_t *out, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RBPR()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::RBPR);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(out, size);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Performs an ULBPR (Global Block-Protection Unlock) operation.
         *
         * This command requires Write Enable Latch (WREN) to be set.
         *
         * @return The return value is of type Stm32Common::HalStatus, indicating the status of the ULBPR operation.
         * @see WREN()
         */
        HalStatus ULBPR() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::ULBPR()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::ULBPR);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Reads data from the security ID space.
         *
         * @param addr The starting address in the security ID space to read from.
         * @param pData Pointer to the buffer where the read data will be stored.
         * @param size The number of bytes to read.
         * @return A `HalStatus` value representing the status of the operation.
         */
        HalStatus RSID(uint16_t addr, uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RSID(0x%08x, %p, %lu)\r\n",
                             addr, &pData, size);
            spi->select();
            memset(pData, 0, size);
            auto ret = spi->transmit(Instruction::RSID);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit_be(addr);
            ret = ret != HalStatus::HAL_OK ? ret : spi->transmit(0xff);
            ret = ret != HalStatus::HAL_OK ? ret : spi->receive(pData, size);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Enters the Deep Power-Down mode (DPD).
         *
         * @return The status of the DPD transmission: `HalStatus`
         */
        HalStatus DPD() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::DPD()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::DPD);
            spi->unselect();
            return ret;
        }


        /**
         * @brief Release from Deep Power-down (RDPD) mode.
         *
         * @return The HalStatus indicating the success or failure of the RDPD operation.
         */
        HalStatus RDPD() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::RDPD()\r\n");
            spi->select();
            auto ret = spi->transmit(Instruction::RDPD);
            spi->unselect();
            return ret;
        }


        bool isComOk() {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::isComOk()\r\n");
            uint8_t jedecId[3] = {};
            RDID(jedecId, sizeof(jedecId));
            return (jedecId[0] == JEDECID::BYTE_0)
                   && (jedecId[1] == JEDECID::BYTE_1)
                   && (jedecId[2] == JEDECID::BYTE_2);
        }

        HalStatus waitForComOk(const uint32_t timeout_ms) {
            const uint32_t start_ms = millis();
            while (!isComOk()) {
                delay(1);
                if ((timeout_ms > 0) && (millis() - start_ms > timeout_ms)) return HalStatus::HAL_TIMEOUT;
            }
            return HalStatus::HAL_OK;
        }

        HalStatus waitForComOk() {
            return waitForComOk(0);
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
        HalStatus getEUI48(uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::getEUI48()\r\n");
            if (size < 6) return HalStatus::HAL_ERROR;
            memset(pData, 0, size);
            if (SFDP(SFDP::EUI48_PROGRAMMED) == 0x30) {
                return SFDP(0x261, pData, 6);
            }
            return HalStatus::HAL_ERROR;
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
        HalStatus getEUI64(uint8_t *pData, const uint16_t size) {
            log()->setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->printf("Stm32LevelX::Driver::Sst26Driver::getEUI64()\r\n");
            if (size < 8) return HalStatus::HAL_ERROR;
            memset(pData, 0, size);
            if (SFDP(SFDP::EUI64_PROGRAMMED) == 0x40) {
                return SFDP(0x268, pData, 8);
            }
            return HalStatus::HAL_ERROR;
        }

    public:
        ULONG getTotalBlocks() override;

        ULONG getBlockSize() override;

        UINT readSector(uint32_t addr, uint8_t *out, uint16_t size) override;

        /**
         * @brief Writes a sector of data to the SST26 flash device.
         *
         * This method writes a sector of data to the SST26 flash device starting from the specified address.
         * The size of the sector must be specified in bytes.
         *
         * @param addr The starting address of the sector in the flash memory.
         * @param in Pointer to the input data to be written.
         * @param size The size of the input data in bytes.
         *
         * @return Returns an @c UINT value indicating the status of the write operation.
         */
        UINT writeSector(uint32_t addr, uint8_t *in, uint16_t size) override;

        UINT reset() override;

    protected:
        Stm32Spi::Spi *spi;
    };
}

#endif
