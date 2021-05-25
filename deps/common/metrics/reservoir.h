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
// Created by Longda on 2021/4/19.
//

#ifndef __COMMON_METRICS_RESERVOIR_H_
#define __COMMON_METRICS_RESERVOIR_H_

#include <stdint.h>

#include "common/math/random_generator.h"
#include "common/metrics/metric.h"
#include "common/metrics/snapshot.h"



namespace common {

class Reservoir : public Metric {
public:
  Reservoir(RandomGenerator &random);
  virtual ~Reservoir();

public:
  virtual size_t size() = 0;
  virtual int getCount() = 0;

  virtual void update(double one) = 0;

  virtual void reset() = 0;

protected:
  virtual int next(int range);

private:
  RandomGenerator &random;
};

} // namespace common

#endif /* __COMMON_METRICS_RESERVOIR_H_ */
