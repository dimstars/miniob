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

#ifndef __COMMON_METRICS_SNAPSHOT_H__
#define __COMMON_METRICS_SNAPSHOT_H__

#include <string>
#include "common/lang/string.h"

namespace common {


class Snapshot {
public:
  virtual ~Snapshot() {};
  virtual std::string toString() = 0;
};

template <class T>
class SnapshotBasic : public Snapshot {
public:
  SnapshotBasic(){

  };

  virtual ~SnapshotBasic() {}

  void setValue(T &input) { value = input; }

  std::string toString() {
    std::string ret;
    val_to_str(value, ret);
    return ret;
  }

private:
  T value;
};

class SimplerTimerSnapshot: public  Snapshot{
public:
   SimplerTimerSnapshot() {

  }

  virtual ~SimplerTimerSnapshot() {}

  void setValue(double mean, double tps) {
    this->mean = mean;
    this->tps = tps;
  }

  std::string toString() {
    std::stringstream oss;
    oss << "mean:" << mean << ",tps:"<<tps;

    return oss.str();
  }
private:
  double mean;
  double tps;
};
} //namespace common
#endif //__COMMON_METRICS_SNAPSHOT_H__
