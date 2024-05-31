/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * STOP Restart.
 * M999
 */

#ifndef STM32F4_UNO_APPLICATION_COMMANDS_M999_HPP
#define STM32F4_UNO_APPLICATION_COMMANDS_M999_HPP

#include "AbstractCommand.hpp"
#include "defines.h"
#include "Helper.hpp"

using namespace Stm32ItmLogger;

class M999 : public Stm32GcodeRunner::AbstractCommand {
public:
    M999() { isSync = true; }

    const char *getName() override {
        return F("M999");
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

        out()->print(F("RESETTING MCU"));
        out()->flush();

        HAL_NVIC_SystemReset();

        result = runReturn::RUNNING;
        return result;
    }

    cleanupReturn cleanup() override {
        auto result = AbstractCommand::cleanup();
        result = cleanupReturn::OK;
        return result;
    }
};

#endif
