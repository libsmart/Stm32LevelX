/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#ifndef STM32F4_UNO_APPLICATION_SERIALGCODE_HPP
#define STM32F4_UNO_APPLICATION_SERIALGCODE_HPP

#include "Stm32Serial.hpp"

class SerialGcode : public Stm32Serial::Stm32Serial {
public:
    explicit SerialGcode(::Stm32Serial::AbstractDriver *driver)
        : Stm32Serial(driver, &Stm32ItmLogger::logger) { ; }

    SerialGcode(::Stm32Serial::AbstractDriver *driver, Stm32ItmLogger::LoggerInterface *logger)
        : Stm32Serial(driver, logger) { ; }

    void loop() override;
};

#endif
