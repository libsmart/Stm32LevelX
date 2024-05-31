/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * This file holds exports for the global variables, defined in globals.cpp.
 * @see globals.cpp
 */

#ifndef AQUADECAY_TANKBOX_APPLICATION_GLOBALS_HPP
#define AQUADECAY_TANKBOX_APPLICATION_GLOBALS_HPP

#include "globals.h"
#include "Stm32ItmLogger.hpp"
#include <cstdint>
#include <LevelXNorFlash.hpp>
#include <Driver/Sst26Driver.hpp>

#include "Stm32Spi.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t dummyCpp;

/**
 * Global logger instance
 */
inline Stm32ItmLogger::Stm32ItmLogger &Logger = Stm32ItmLogger::logger;

inline Stm32Gpio::PinDigitalOut pinSpi1Nss(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, true);

inline Stm32Spi::Spi spi(&hspi1, &pinSpi1Nss, &Stm32ItmLogger::logger);

inline Stm32LevelX::Driver::Sst26Driver sst26(&spi, &Stm32ItmLogger::logger);

inline Stm32LevelX::LevelXNorFlash LX(&sst26, &Stm32ItmLogger::logger);

#ifdef __cplusplus
}
#endif

#endif //AQUADECAY_TANKBOX_APPLICATION_GLOBALS_HPP
