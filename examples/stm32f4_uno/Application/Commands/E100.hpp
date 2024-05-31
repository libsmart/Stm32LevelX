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
        out()->printf("START_ADDRESS: %d\r\n", A);
        out()->printf("SIZE: %d\r\n", S);


        constexpr uint16_t BUFFER_SIZE = 16;
        const uint16_t SLICES = (S + BUFFER_SIZE - 1) / BUFFER_SIZE;
        uint8_t buffer[BUFFER_SIZE];

        for (uint16_t idx = 0; idx < SLICES; idx++) {
            // const uint16_t sz = std::min(S - idx * BUFFER_SIZE, static_cast<int32_t>(BUFFER_SIZE));
            sst26.read(A + idx * BUFFER_SIZE, buffer, BUFFER_SIZE);
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


    runReturn run() override {
        auto result = AbstractCommand::run();

        out()->println(F("FLASH INSPECTOR"));
        out()->printf("COMMAND: %s\r\n", C);

        if (strcmp(C, "dump") == 0) {
            result = runDump();
        }

        out()->println();

        result = runReturn::FINISHED;
        return result;
    }

    cleanupReturn cleanup() override {
        auto result = AbstractCommand::cleanup();
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
            default:
                break;
        }
        AbstractCommand::setParam(paramName, paramString);
    }

private:
    int32_t S = -1;
    int32_t A = -1;
    char C[10] = {};
};

#endif
