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

#ifndef __COMMON_METRICS_HISTOGRAM_SNAPSHOT_H_
#define __COMMON_METRICS_HISTOGRAM_SNAPSHOT_H_

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "common/metrics/snapshot.h"

namespace common {

class HistogramSnapShot : public Snapshot {
public:
  HistogramSnapShot();
  explicit HistogramSnapShot(const std::vector<double> &collection);
  virtual ~HistogramSnapShot();

public:
  void set_collection(const std::vector<double> &collection);

  /**
   * Returns the value at the given quantile
   *
   * @param quantile a given quantile, in {@code [0..1]}
   * @return the value in the distribute
   */
  double get_value(double quantile);

  /**
   * Returns the size of collection in the snapshot
   */
  size_t size() const;

  /**
   * Returns 50th_percentile.
   */
  double get_median();

  double get_75th();
  double get_90th();
  double get_95th();
  double get_99th();
  double get_999th();

  double get_max();
  double get_min();
  double get_mean();

  const std::vector<double> &get_values();

  std::string to_string();
protected:
  std::vector<double> data_;
};

} // namespace common

#endif /* __COMMON_METRICS_HISTOGRAM_SNAPSHOT_H_ */
