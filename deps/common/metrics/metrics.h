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

#ifndef __COMMON_METRICS_METRICS_H__
#define __COMMON_METRICS_METRICS_H__

#include "common/lang/string.h"
#include "common/metrics/metric.h"
#include "common/metrics/snapshot.h"
#include "common/metrics/timer_snapshot.h"
#include "common/metrics/uniform_reservoir.h"
#include <sys/time.h>

namespace common {

class Gauge : public Metric {
public:
  // user implement snapshot function
  void setSnapshot(Snapshot *value) { snapshotValue = value; }
};

class Counter : public Metric {
  void setSnapshot(SnapshotBasic<long> *value) { snapshotValue = value; }
};

class Meter : public Metric {
public:
  Meter();
  virtual  ~Meter();

  void inc(long increase);
  void inc();

  void snapshot();

protected:
  std::atomic<long> value;
  long snapshotTick;
};

// SimpleTimer just get tps and meanvalue
// time unit is ms
class SimpleTimer : public Meter {
public:
  virtual ~SimpleTimer();

  void inc(long increase);

  void update(long one);

  void snapshot();

protected:
  std::atomic<long> times;
};

// Histogram metric is complicated, in normal case ,
//  please skip us histogram or Timer as more as possible
//  try use SimpleTimer to replace them.
//  if use histogram , please use sampling method.
class Histogram : public UniformReservoir {
public:
  Histogram(RandomGenerator &random);
  Histogram(RandomGenerator &random, size_t size);
  virtual ~Histogram();

  void snapshot();

};

// timeunit is ms
// Timer = Histogram + Meter
class Timer : public UniformReservoir {
public:
  Timer(RandomGenerator &random);
  Timer(RandomGenerator &random, size_t size);
  virtual ~Timer();

  void snapshot();
  void update(double ms);

protected:
  std::atomic<long> value;
  long snapshotTick;
};
// update ms
class TimerStat {
public:
  TimerStat(SimpleTimer &st);

  ~TimerStat();
  void start();
  void end();

public:
  SimpleTimer &st;
  long startTick;
  long endTick;
};

} // namespace common
#endif //__COMMON_METRICS_METRICS_H__
