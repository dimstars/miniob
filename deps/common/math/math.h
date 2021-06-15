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

#ifndef __COMMON_MATH_MATH_H__
#define __COMMON_MATH_MATH_H__
namespace common {

/**
 * give seed for random number generation
 * TODO: the implementation of rand() in glibc is thread-safe,
 *       but it will take a global lock to protect static data structure.
 *       could consider using XrandNN_r() later
 */
void seed_random();

/**
 * if scope is bigger than RAND_MAX/10, the precision is not good
 */
long int lrandom(const long int scope);

} //namespace common
#endif /* __COMMON_MATH_MATH_H__ */
