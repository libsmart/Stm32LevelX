/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * This file holds exports for the global variables, defined in globals.cpp.
 * @see globals.cpp
 */

#ifndef STM32F4_UNO_APPLICATION_GLOBALS_HPP
#define STM32F4_UNO_APPLICATION_GLOBALS_HPP

#include "globals.h"
#include "Stm32ItmLogger.hpp"
#include <cstdint>
#include <LevelXNorFlash.hpp>
#include <Driver/Sst26Driver.hpp>

#include "SerialGcode.hpp"
#include "Stm32Serial.hpp"
#include "Stm32Spi.hpp"
#include "usart.h"
#include "Driver/Stm32HalUartItDriver.hpp"

#include "Store.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t dummyCpp;

/**
 * Global logger instance
 */
inline Stm32ItmLogger::Stm32ItmLogger &Logger = Stm32ItmLogger::logger;

inline Stm32Gpio::PinDigitalOut pinSpi1Nss(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, true);

inline Stm32Spi::Spi spi(&hspi1, &pinSpi1Nss/*, &Stm32ItmLogger::logger*/);

inline Stm32LevelX::Driver::Sst26Driver sst26(&spi/*, &Stm32ItmLogger::logger*/);

inline Stm32LevelX::LevelXNorFlash LX(&sst26, &Stm32ItmLogger::logger);

inline ULONG sector[LX_NOR_SECTOR_SIZE] = {};
inline char *str = (char *) sector;

inline Stm32Serial::Stm32HalUartItDriver Uart1SerialDriver(&huart1, "Uart1SerialDriver");
inline SerialGcode Serial1(&Uart1SerialDriver);

struct structV1 {
    uint8_t version = 1;
    uint32_t aNumber = 43287;
    char text[20] = {"Hello World"};
};

constexpr uint32_t structV1_logicalSector = 5;
inline Stm32LevelX::Store<structV1> storeV1(&LX, structV1_logicalSector, &Stm32ItmLogger::logger);


struct structV2 {
    uint8_t version = 2;
    uint32_t aNumber = 43287;
    char text1[26] = {"Hello World"};
    char text2[26] = {"Hello World"};
    char text3[26] = {"Hello World"};
    char text4[26] = {"Hello World"};
    char text5[100] = {"Hello World"};
    char text6[100] = {"Hello World"};
    char text7[100] = {"Hello World"};
    char text8[100] = {"Hello World"};
    char text9[100] = {"Hello World"};
};

constexpr uint32_t structV2_logicalSector = 6;
inline Stm32LevelX::Store<structV2> storeV2(&LX, structV2_logicalSector, &Stm32ItmLogger::logger);


#ifdef __cplusplus
}
#endif

#endif
