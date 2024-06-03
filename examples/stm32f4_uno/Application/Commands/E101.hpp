/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * LevelX inspector.
 * E101
 */

#ifndef STM32F4_UNO_APPLICATION_COMMANDS_E101_HPP
#define STM32F4_UNO_APPLICATION_COMMANDS_E101_HPP

#include "AbstractCommand.hpp"
#include "defines.h"
#include "globals.hpp"
#include "Helper.hpp"

using namespace Stm32ItmLogger;

class E101 : public Stm32GcodeRunner::AbstractCommand {
public:
    E101() { isSync = true; }

    const char *getName() override {
        return F("E101");
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


    runReturn runInitialize() {
        out()->printf("LEVELX_INITIALIZE: ");
        const auto ret = LX.initialize();
        out()->printf("0x%02x %s\r\n", ret, Stm32LevelX::LevelXNorFlash::getErrorCodeString(ret));
        return ret == Stm32LevelX::LevelXErrorCode::SUCCESS ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn runOpen() {
        out()->printf("LEVELX_OPEN: ");
        const auto ret = LX.open();
        out()->printf("0x%02x %s\r\n", ret, Stm32LevelX::LevelXNorFlash::getErrorCodeString(ret));
        return ret == Stm32LevelX::LevelXErrorCode::SUCCESS ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn runClose() {
        out()->printf("LEVELX_CLOSE: ");
        const auto ret = LX.close();
        out()->printf("0x%02x %s\r\n", ret, Stm32LevelX::LevelXNorFlash::getErrorCodeString(ret));
        return ret == Stm32LevelX::LevelXErrorCode::SUCCESS ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn runReadSector() {
        A = std::max(A, static_cast<int32_t>(0));
        out()->printf("LEVELX_SECTOR_READ: ");
        const auto ret = LX.sectorRead(A, sector);
        out()->printf("0x%02x %s\r\n", ret, Stm32LevelX::LevelXNorFlash::getErrorCodeString(ret));
        if (ret != Stm32LevelX::LevelXErrorCode::SUCCESS) return runReturn::ERROR;

        constexpr uint16_t LEVELX_SECTOR_SIZE = Stm32LevelX::LevelXNorFlash::getSectorSize();
        constexpr uint16_t BUFFER_SIZE = 16;
        constexpr uint16_t SLICES = (LEVELX_SECTOR_SIZE + BUFFER_SIZE - 1) / BUFFER_SIZE;

        for (uint16_t idx = 0; idx < SLICES; idx++) {
            const uint32_t addr = idx * BUFFER_SIZE;
            out()->printf(" 0x%04x:0x%04x |", A, addr);
            for (uint16_t ci = 0; ci < BUFFER_SIZE; ci++) {
                const uint8_t c = sectorBytewise[addr + ci];
                out()->printf(" %02x", c);
            }
            out()->print(" | ");
            for (uint16_t ci = 0; ci < BUFFER_SIZE; ci++) {
                const uint8_t c = sectorBytewise[addr + ci];
                out()->printf("%c", c >= ' ' && c <= 127 ? c : '.');
            }
            out()->println();
            delay(20);
        }

        return runReturn::FINISHED;
    }


    runReturn runReleaseSector() {
        A = std::max(A, static_cast<int32_t>(0));
        out()->printf("LEVELX_SECTOR_RELEASE: ");
        const auto ret = LX.sectorRelease(A);
        out()->printf("0x%02x %s\r\n", ret, Stm32LevelX::LevelXNorFlash::getErrorCodeString(ret));
        return ret == Stm32LevelX::LevelXErrorCode::SUCCESS ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn runWriteSector() {
        A = std::max(A, static_cast<int32_t>(0));
        out()->printf("LEVELX_SECTOR_WRITE: ");
        memset(sector, 0, sizeof(sector));
        snprintf(reinterpret_cast<char *>(sector), sizeof(sector), "%s", W);
        const auto ret = LX.sectorWrite(A, sector);
        out()->printf("0x%02x %s\r\n", ret, Stm32LevelX::LevelXNorFlash::getErrorCodeString(ret));
        return ret == Stm32LevelX::LevelXErrorCode::SUCCESS ? runReturn::FINISHED : runReturn::ERROR;
    }


    runReturn run() override {
        auto result = AbstractCommand::run();

        out()->println(F("LEVELX INSPECTOR"));
        out()->printf("COMMAND: %s\r\n", C);

        if (strcmp(C, "init") == 0) {
            result = runInitialize();
        }
        if (strcmp(C, "open") == 0) {
            result = runOpen();
        }
        if (strcmp(C, "close") == 0) {
            result = runClose();
        }
        if (strcmp(C, "read") == 0) {
            result = runReadSector();
        }
        if (strcmp(C, "rel") == 0) {
            result = runReleaseSector();
        }
        if (strcmp(C, "write") == 0) {
            result = runWriteSector();
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
        memset(sector, 0, sizeof(sector));

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
    char C[15] = {};
    char W[21] = {};
    ULONG sector[LX_NOR_SECTOR_SIZE] = {};
    uint8_t *sectorBytewise = reinterpret_cast<uint8_t *>(sector);
};

#endif
