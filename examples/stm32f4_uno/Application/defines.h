/*
 * SPDX-FileCopyrightText: 2024 Roland Rusch, easy-smart solution GmbH <roland.rusch@easy-smart.ch>
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/**
 * This file holds global macro defines. It can be imported in C and C++ files.
 * Do not put global variables in this file.
 * @see globals.cpp
 * @see globals.c
 */


#ifndef AQUADECAY_TANKBOX_APPLICATION_DEFINES_H
#define AQUADECAY_TANKBOX_APPLICATION_DEFINES_H

/** Name of the firmware. */
#define FIRMWARE_NAME "AquaDecay-TankBox"

/** Version of the firmware. */
#ifndef FIRMWARE_VERSION
#ifdef VER_NEXT_ALPHA
#define FIRMWARE_VERSION VER_NEXT_ALPHA
#else
#define FIRMWARE_VERSION "0.0.0-alpha+UNKNOWN"
#endif
#endif

#endif //AQUADECAY_TANKBOX_APPLICATION_DEFINES_H