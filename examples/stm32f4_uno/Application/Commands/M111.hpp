/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * Debug Level.
 * M111 [S<lvl>]
 */

#ifndef STM32F4_UNO_APPLICATION_COMMANDS_M115_HPP
#define STM32F4_UNO_APPLICATION_COMMANDS_M115_HPP

#include "AbstractCommand.hpp"
#include "defines.h"
#include "Helper.hpp"

using namespace Stm32ItmLogger;

class M111 : public Stm32GcodeRunner::AbstractCommand {
public:
    M111() { isSync = true; }

    const char *getName() override {
        return F("M111");
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

    runReturn run() override {
        auto result = AbstractCommand::run();

        if (S >= 0) {
            logger.setPrintSeverity(static_cast<LoggerInterface::Severity>(S));
        }

        const long sev = static_cast<long>(logger.getPrintSeverity());
        out()->print(F("DEBUG_LEVEL: "));
        out()->println(sev);
        out()->flush();


        out()->print("INFO: EMERGENCY (1)      ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::EMERGENCY) ? "ON" : "OFF");
        out()->print("INFO: ALERT (2)          ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::ALERT) ? "ON" : "OFF");
        out()->print("INFO: CRITICAL (4)       ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::CRITICAL) ? "ON" : "OFF");
        out()->print("INFO: ERROR (8)          ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::ERROR) ? "ON" : "OFF");
        out()->print("INFO: WARNING (16)       ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::WARNING) ? "ON" : "OFF");
        out()->print("INFO: NOTICE (32)        ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::NOTICE) ? "ON" : "OFF");
        out()->print("INFO: INFORMATIONAL (64) ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::INFORMATIONAL) ? "ON" : "OFF");
        out()->print("INFO: DEBUGGING (128)    ");
        out()->println(sev & static_cast<uint8_t>(LoggerInterface::Severity::DEBUGGING) ? "ON" : "OFF");
        out()->flush();

        result = runReturn::FINISHED;
        return result;
    }

    cleanupReturn cleanup() override {
        auto result = AbstractCommand::cleanup();
        result = cleanupReturn::OK;
        return result;
    }

    void setParam(char paramName, long paramLong) override {
        if (paramName == 'S') {
            S = paramLong;
        }
    }

private:
    int32_t S = -1;
};

#endif
