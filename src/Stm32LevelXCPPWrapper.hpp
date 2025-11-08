/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBSMART_STM32LEVELX_STM32LEVELXCPPWRAPPER_HPP
#define LIBSMART_STM32LEVELX_STM32LEVELXCPPWRAPPER_HPP


#ifdef __cplusplus
extern "C" {
#endif

#include "lx_api.h"

extern UINT Stm32LevelX_setup(TX_BYTE_POOL *byte_pool);

#ifdef __cplusplus
}
#endif


#endif //LIBSMART_STM32LEVELX_STM32LEVELXCPPWRAPPER_HPP
