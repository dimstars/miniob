/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

//
// Created by Longda on 2010
//

#ifndef __COMMON_OS_SIGNAL_H__
#define __COMMON_OS_SIGNAL_H__

#include <signal.h>

namespace common {

//! Default function that blocks signals.
/**
 * Now it blocks SIGINT, SIGTERM, and SIGUSR1
 */
void blockDefaultSignals(sigset_t *signal_set, sigset_t *old_set);
//! Default function that unblocks signals.
/**
 * It unblocks SIGINT, SIGTERM,and SIGUSR1.
 */
void unBlockDefaultSignals(sigset_t *signal_set, sigset_t *old_set);


void *waitForSignals(sigset_t *signal_set);
void startWaitForSignals(sigset_t *signal_set);

// Set signal handling function
/**
 * handler function
 */
typedef void (*sighandler_t)(int);
void setSignalHandler(sighandler_t func);
void setSignalHandler(int sig, sighandler_t func);

} //namespace common
#endif /* __COMMON_OS_SIGNAL_H__ */
