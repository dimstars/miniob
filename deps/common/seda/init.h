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

#ifndef __COMMON_SEDA_INIT_H__
#define __COMMON_SEDA_INIT_H__

// Basic includes
#include <assert.h>
#include <iostream>
#include <set>
#include <signal.h>
#include <sstream>
#include <string>
#include <vector>

#include "common/conf/ini.h"
#include "common/defs.h"
#include "common/os/process_param.h"
namespace common {



/**
 * start the seda process, do this will trigger all threads
 */
int initSeda(ProcessParam *pProcessCfg);

void cleanupSeda();

} //namespace common
#endif // __COMMON_SEDA_INIT_H__
