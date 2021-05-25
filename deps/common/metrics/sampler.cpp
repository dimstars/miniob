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

Sampler *&theGlobalSampler() {
  static Sampler *gSampler = new Sampler();

  return gSampler;
}

Sampler::Sampler():random() {}

Sampler::~Sampler() {}

bool Sampler::sampling() {
  int v = random.next(RANGE_SIZE);
  if (v <= ratioNum) {
    return true;
  } else {
    return false;
  }
}

double Sampler::getRatio() { return ratio; }

void Sampler::setRatio(double ratio) {
  if (0 <= ratio && ratio <= 1) {
    this->ratio = ratio;
    ratioNum = ratio * RANGE_SIZE;
  } else {
    LOG_WARN("Invalid ratio :%lf", ratio);
  }
}

}//namespace common

