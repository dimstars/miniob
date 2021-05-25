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

#ifndef __COMMON_VERSION_H__
#define __COMMON_VERSION_H__
namespace common {

#ifndef MAIJOR_VER
#define MAIJOR_VER 1
#endif

#ifndef MINOR_VER
#define MINOR_VER 0
#endif

#ifndef PATCH_VER
#define PATCH_VER 0
#endif

#ifndef OTHER_VER
#define OTHER_VER 1
#endif

#define STR1(R) #R
#define STR2(R) STR1(R)

#define VERSION_STR                                                            \
  (STR2(MAIJOR_VER) "." STR2(MINOR_VER) "." STR2(PATCH_VER) "." STR2(OTHER_VER))
#define VERSION_NUM                                                            \
  (MAIJOR_VER << 24 | MINOR_VER << 16 | PATCH_VER << 8 | OTHER_VER)

} //namespace common
#endif //__COMMON_VERSION_H__
