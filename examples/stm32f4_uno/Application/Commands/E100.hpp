/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * Flash inspector.
 * E100
 */

#ifndef STM32F4_UNO_APPLICATION_COMMANDS_E100_HPP
#define STM32F4_UNO_APPLICATION_COMMANDS_E100_HPP

#include "AbstractCommand.hpp"
#include "defines.h"
#include "globals.hpp"
#include "Helper.hpp"

using namespace Stm32ItmLogger;

class E100 : public Stm32GcodeRunner::AbstractCommand {
public:
    E100() { isSync = true; }

    const char *getName() override {
        return F("E100");
    }

    preFlightCheckReturn preFlightCheck() override {
        auto result = AbstractCommand::preFlightCheck();
        result = preFlightCheckReturn::READY;
        return result;
    }

    initReturn init() override {
        auto result = AbstractCommand::init();
        result = initReturn::READY;
        return result;
    }


    runReturn runDump() {
        A = std::max(A, static_cast<int32_t>(0));
        out()->printf("START_ADDRESS: 0x%08x\r\n", A);
        S = S < 0 ? 512 : S;
        out()->printf("SIZE: %d\r\n", S);


        constexpr uint16_t BUFFER_SIZE = 16;
        const uint16_t SLICES = (S + BUFFER_SIZE - 1) / BUFFER_SIZE;
        uint8_t buffer[BUFFER_SIZE];

        for (uint16_t idx = 0; idx < SLICES; idx++) {
            // const uint16_t sz = std::min(S - idx * BUFFER_SIZE, static_cast<int32_t>(BUFFER_SIZE));
            const uint32_t addr = A + idx * BUFFER_SIZE;
            out()->printf(" 0x%08x |", addr);
            const auto ret = sst26.READ(addr, buffer, BUFFER_SIZE);
            if (ret != HalStatus::HAL_OK) return runReturn::ERROR;
            for (const char c: buffer) {
                out()->printf(" %02x", c);
            }
            out()->print(" | ");
            for (const char c: buffer) {
                out()->printf("%c", c >= ' ' && c <= 127 ? c : '.');
            }
            out()->println();
            delay(20);
        }

        return runReturn::FINISHED;
    }

    runReturn runWrite() {
        A = std::max(A, static_cast<int32_t>(0));
        out()->printf("START_ADDRESS: 0x%08x\r\n", A);

        auto ret = sst26.WREN();
        ret = ret != HalStatus::HAL_OK ? ret : sst26.PP(A, reinterpret_cast<uint8_t *>(W), strlen(W));
        ret = ret != HalStatus::HAL_OK ? ret : sst26.waitForWriteFinish(3);
        ret = ret != HalStatus::HAL_OK ? ret : sst26.WRDI();

        return ret == HalStatus::HAL_OK ? runReturn::FINISHED : runReturn::ERROR;
    }

    runReturn runCheck() {
        const auto SECTOR_SIZE = static_cast<int32_t>(sst26.getSectorSize());
        out()->printf("START_ADDRESS: 0x%08x\r\n", A < 0 ? 0 : A);
        out()->printf("SECTOR_SIZE: %d\r\n", SECTOR_SIZE);

        uint32_t sectorsErased = 0;
        uint32_t sectorsNotErased = 0;

        for (uint32_t sector = A >= 0 ? A / SECTOR_SIZE : 0; sector < sst26.getTotalSectors(); sector++) {
            const uint32_t addr = sector * SECTOR_SIZE;
            out()->printf("SECTOR_ADDRESS (SECTOR_NUMBER): 0x%08x (%d) : ", addr, sector);
            // out()->printf("SECTOR_NUMBER: %d\r\n", sector);
            auto ret = sst26.verifySectorErased(addr);
            if (ret == LX_SUCCESS) {
                out()->printf("1\r\n");
                sectorsErased++;
            } else {
                out()->printf("0\r\n");
                sectorsNotErased++;
            }
            if (A >= 0) return runReturn::FINISHED;
            delay(20);
        }
        out()->printf("SECTORS_CHECKED: %d\r\n", sectorsErased + sectorsNotErased);
        out()->printf("SECTORS_ERASED: %d\r\n", sectorsErased);
        out()->printf("SECTORS_NOT_ERASED: %d\r\n", sectorsNotErased);

        return runReturn::FINISHED;
    }


    runReturn runErase() {
        out()->printf("SECTOR_ADDRESS: 0x%08x\r\n", A);
        auto ret = sst26.WREN();
        ret = ret != HalStatus::HAL_OK ? ret : sst26.SE(A);
        ret = ret != HalStatus::HAL_OK ? ret : sst26.waitForWriteFinish(50);
        ret = ret != HalStatus::HAL_OK ? ret : sst26.WRDI();

        return ret == HalStatus::HAL_OK ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn runChipErase() {
        auto ret = sst26.WREN();
        ret = ret != HalStatus::HAL_OK ? ret : sst26.CE();
        ret = ret != HalStatus::HAL_OK ? ret : sst26.waitForWriteFinish(100);
        ret = ret != HalStatus::HAL_OK ? ret : sst26.WRDI();

        return ret == HalStatus::HAL_OK ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn run() override {
        auto result = AbstractCommand::run();

        out()->println(F("FLASH INSPECTOR"));
        out()->printf("COMMAND: %s\r\n", C);

        if (strcmp(C, "dump") == 0) {
            result = runDump();
        }
        if (strcmp(C, "write") == 0) {
            result = runWrite();
        }
        if (strcmp(C, "check") == 0) {
            result = runCheck();
        }
        if (strcmp(C, "erase") == 0) {
            result = runErase();
        }
        if (strcmp(C, "chiperase") == 0) {
            result = runChipErase();
        }


        out()->println();

        // result = runReturn::FINISHED;
        return result;
    }

    cleanupReturn cleanup() override {
        auto result = AbstractCommand::cleanup();

        S = -1;
        A = -1;
        memset(C, 0, sizeof(C));
        memset(W, 0, sizeof(W));

        result = cleanupReturn::OK;
        return result;
    }

    void setParam(char paramName, long paramLong) override {
        switch (paramName) {
            case 'S':
                S = paramLong;
                break;
            case 'A':
                A = paramLong;
                break;
            default:
                break;
        }
    }

    int findFirstOf(const char *str, const char *chars) {
        for (int i = 0; str[i] != '\0'; i++) {
            for (int j = 0; chars[j] != '\0'; j++) {
                if (str[i] == chars[j]) {
                    return i;
                }
            }
        }
        return -1;
    }

    void setParam(char paramName, const char *paramString) override {
        const auto pos = findFirstOf(paramString, " \n\r\t\v\x00");

        switch (paramName) {
            case 'C':
                snprintf(C, sizeof(C), "%.*s", pos, paramString);
                break;
            case 'W':
                snprintf(W, sizeof(W), "%.*s", pos, paramString);
                break;

            default:
                break;
        }
        AbstractCommand::setParam(paramName, paramString);
    }

private:
    int32_t S = -1;
    int32_t A = -1;
    char C[16] = {};
    char W[51] = {};
};

#endif
