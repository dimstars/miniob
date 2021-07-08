/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
OceanBase is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

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
