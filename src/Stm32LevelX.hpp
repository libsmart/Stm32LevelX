/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_STM32LEVELX_HPP
#define LIBSMART_STM32LEVELX_STM32LEVELX_HPP

#include <libsmart_config.hpp>

#include "Stm32ItmLogger.hpp"
#include "Loggable.hpp"
#include "lx_api.h"

namespace Stm32LevelX {
    class LevelX : public Stm32ItmLogger::Loggable {
    public:
        static UINT setup(TX_BYTE_POOL *byte_pool) {
            Stm32ItmLogger::logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::INFORMATIONAL)
                    ->println("Stm32LevelX::LevelX::setup()");

            return TX_SUCCESS;
        }
    };
}

#endif //LIBSMART_STM32LEVELX_STM32LEVELX_HPP
