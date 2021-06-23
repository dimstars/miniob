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

#include "common/metrics/sampler.h"
#include "common/log/log.h"

#define RANGE_SIZE 100

namespace common {

Sampler *&get_sampler() {
  static Sampler *g_sampler = new Sampler();

  return g_sampler;
}

Sampler::Sampler():random_() {}

Sampler::~Sampler() {}

bool Sampler::sampling() {
  int v = random_.next(RANGE_SIZE);
  if (v <= ratio_num_) {
    return true;
  } else {
    return false;
  }
}

double Sampler::get_ratio() { return ratio_; }

void Sampler::set_ratio(double ratio) {
  if (0 <= ratio && ratio <= 1) {
    this->ratio_ = ratio;
    ratio_num_ = ratio * RANGE_SIZE;
  } else {
    LOG_WARN("Invalid ratio :%lf", ratio);
  }
}

}//namespace common

