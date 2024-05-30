/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * This file holds the main setup() and loop() functions for C++ code.
 * If a RTOS is used, loop() is called in the main task and setup() is called before RTOS initialization.
 * @see App_ThreadX_Init() in Core/Src/app_threadx.c
 */

#include "main.hpp"
#include "globals.hpp"
#include "RunEvery.hpp"


/**
 * @brief Setup function.
 * This function is called once at the beginning of the program before ThreadX is initialized.
 * @see main() in Core/Src/main.c
 */
void setup() {
    dummyCpp = 0;
    dummyCandCpp = 0;


    pinSpi1Nss.setup();


    Logger.printf("spi.getState() = 0x%08x\r\n", spi.getState());
    Logger.printf("spi.getError() = 0x%08x\r\n", spi.getError());


    uint8_t spi_buf[8];

    spi.select();
    spi.transmit((uint8_t) 0x9f); // RDID

    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 3);
    spi.unselect();

    Logger.printf("SST26 JEDEC ID: 0x%02x 0x%02x 0x%02x\r\n",
                  (uint8_t) spi_buf[0], (uint8_t) spi_buf[1], (uint8_t) spi_buf[2]);
    if ((spi_buf[0] != 0xbf) || (spi_buf[1] != 0x26) || (spi_buf[2] != 0x41)) {
        Logger.setSeverity(Stm32ItmLogger::LoggerInterface::Severity::ERROR)
                ->println("ERROR: NO SST26 chip detected!");
    }

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    spi.transmit((uint32_t) 0xff600200);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x260: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    spi.transmit("\x00\x02\x61\xff", 4);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x261: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    spi.transmit_be((uint32_t) 0x262 << 8 | 0xff);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x262: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    uint32_t addr = 0x263;
    uint8_t tmp;
    tmp=((addr & 0xFFFFFF) >> 16);
    spi.transmit(tmp);
    tmp=(((addr & 0xFFFF) >> 8));
    spi.transmit(tmp);
    tmp=(addr & 0xFF);
    spi.transmit(tmp);
    tmp=0xFF;
    spi.transmit(tmp);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x263: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    spi.transmit_be((uint32_t) 0x264 << 8 | 0xff);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x264: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    spi.transmit_be((uint32_t) 0x265 << 8 | 0xff);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x265: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

    spi.select();
    spi.transmit((uint8_t) 0x5a); // SFDP
    spi.transmit_be((uint32_t) 0x266 << 8 | 0xff);
    memset(spi_buf, 0, sizeof(spi_buf));
    spi.receive(spi_buf, 1);
    Logger.printf("SFDP 0x266: 0x%02x\r\n", spi_buf[0]);
    spi.unselect();

}


/**
 * @brief This function is the main loop that executes continuously.
 * The function is called inside the mainLoopThread().
 * @see mainLoopThread() in AZURE_RTOS/App/app_azure_rtos.c
 */
void loop() {
    static Stm32Common::RunEvery re2(300);
    re2.loop([]() {
        HAL_GPIO_WritePin(LED1_GRN_GPIO_Port, LED1_GRN_Pin, dummyCpp & 1 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED2_ORG_GPIO_Port, LED2_ORG_Pin, dummyCpp & 2 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED3_RED_GPIO_Port, LED3_RED_Pin, dummyCpp & 4 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED4_BLU_GPIO_Port, LED4_BLU_Pin, dummyCpp & 8 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        // Logger.printf("counter = %d\r\n", dummyCpp);
        dummyCpp++;
        dummyCandCpp++;
    });
}


/**
 * @brief This function handles fatal errors.
 * @see Error_Handler() in Core/Src/main.c
 */
void errorHandler() {
    while (true) {
        //        for (uint32_t i = (SystemCoreClock / 10); i > 0; i--) { UNUSED(i); }
    }
}
