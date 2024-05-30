/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#ifndef AQUADECAY_TANKBOX_APPLICATION_MAINTHREAD_HPP
#define AQUADECAY_TANKBOX_APPLICATION_MAINTHREAD_HPP

#include "app_threadx.h"

#define MAIN_THREAD_STACK_SIZE 2048

#ifdef __cplusplus
extern "C" {
#endif

    UINT setupMainThread(TX_BYTE_POOL *byte_pool);

#ifdef __cplusplus
}
#endif

#endif //AQUADECAY_TANKBOX_APPLICATION_MAINTHREAD_HPP