/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * Report Settings.
 * M503
 */

#ifndef STM32F4_UNO_APPLICATION_COMMANDS_M503_HPP
#define STM32F4_UNO_APPLICATION_COMMANDS_M503_HPP

#include "AbstractCommand.hpp"
#include "defines.h"
#include "Helper.hpp"

using namespace Stm32ItmLogger;

class M503 : public Stm32GcodeRunner::AbstractCommand {
public:
    M503() { isSync = true; }

    const char *getName() override {
        return F("M503");
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

        out()->print(F("DEBUG_LEVEL: "));
        out()->println(static_cast<long>(logger.getPrintSeverity()));
        out()->flush();

        result = runReturn::FINISHED;
        return result;
    }

    cleanupReturn cleanup() override {
        auto result = AbstractCommand::cleanup();
        result = cleanupReturn::OK;
        return result;
    }
};

#endif
