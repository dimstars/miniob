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
#ifndef __COMMON_METRICS_UNIFORM_RESERVOIR_H_
#define __COMMON_METRICS_UNIFORM_RESERVOIR_H_

#include <atomic>
#include <vector>
#include <pthread.h>

#include "common/metrics/reservoir.h"

namespace common {

/**
 * A random sampling reservoir of a stream of {@code long}s. Uses Vitter's
 * Algorithm R to produce a statistically representative sample.
 *
 * @see <a href="http://www.cs.umd.edu/~samir/498/vitter.pdf">Random Sampling
 * with a Reservoir</a>
 */

class UniformReservoir : public Reservoir {
public:
  UniformReservoir(RandomGenerator &random);
  UniformReservoir(RandomGenerator &random, size_t size);
  virtual ~UniformReservoir();

public:
  size_t size();  // data buffer size
  int getCount(); // how many items have been insert?

  void update(double one);
  void snapshot();

  void reset();

protected:
  void init(size_t size);

protected:
  pthread_mutex_t mutex;
  int counter; // counter is likely to be bigger than data.size()
  std::vector<double> data;
  RandomGenerator random;
};

} // namespace common

#endif /* __COMMON_METRICS_UNIFORM_RESERVOIR_H_ */
