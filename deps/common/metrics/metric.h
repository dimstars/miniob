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

#ifndef __COMMON_METRICS_METRIC_H__
#define __COMMON_METRICS_METRIC_H__

#include "common/metrics/snapshot.h"

namespace common {

class Metric {
public:
  virtual void snapshot() = 0;

  virtual Snapshot *get_snapshot() { return snapshot_value_; }

protected:
  Snapshot *snapshot_value_;
};

}//namespace common
#endif //__COMMON_METRICS_METRIC_H__
