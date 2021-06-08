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
// Created by Longda on 2021/4/20.
//

#ifndef __COMMON_METRICS_SAMPLER_H__
#define __COMMON_METRICS_SAMPLER_H__

#include "common/math/random_generator.h"

namespace common {


/**
 * The most simple sample function
 */
class Sampler {
public:
  Sampler();
  virtual ~Sampler();

  bool sampling();

  void set_ratio(double ratio);
  double get_ratio();

private:
  double ratio_;
  int ratio_num_;
  RandomGenerator random_;
};

Sampler *&get_sampler();
} //namespace common
#endif //__COMMON_METRICS_SAMPLER_H__
