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

#include "common/metrics/metrics.h"
#include "common/lang/mutex.h"

namespace common {
Meter::Meter() {
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  snapshot_tick_ = start_time.tv_sec * 1000000 + start_time.tv_usec;
  value_.store(0l);
}

Meter::~Meter() {
  if (snapshot_value_ != NULL) {
    delete snapshot_value_;
    snapshot_value_ = NULL;
  }
}

void Meter::inc(long increase) { value_.fetch_add(increase); }
void Meter::inc() { inc(1l); }

void Meter::snapshot() {
  // TODO lock here

  struct timeval now;
  gettimeofday(&now, NULL);

  long now_tick = now.tv_sec * 1000000 + now.tv_usec;

  double temp_value =
      ((double)value_.exchange(0l)) / ((now_tick - snapshot_tick_ ) / 1000000);
  snapshot_tick_ = now_tick;

  if (snapshot_value_ == NULL) {
    snapshot_value_ = new SnapshotBasic<double>();
  }
  ((SnapshotBasic<double> *)snapshot_value_)->setValue(temp_value);
}

SimpleTimer::~SimpleTimer() {
  if (snapshot_value_ != NULL) {
    delete snapshot_value_;
    snapshot_value_ = NULL;
  }
}

void SimpleTimer::inc(long increase) {
  value_.fetch_add(increase);
  times_.fetch_add(1);
}

void SimpleTimer::update(long one) { inc(one); }

void SimpleTimer::snapshot() {

  // TODO lock here
  struct timeval now;
  gettimeofday(&now, NULL);

  long now_tick = now.tv_sec * 1000000 + now.tv_usec;

  long value_snapshot = value_.exchange(0l);
  long times_snapshot = times_.exchange(0l);

  double tps = 0;
  double mean = 0;

  if (times_snapshot > 0) {
    tps = ((double)times_snapshot )/ ((now_tick - snapshot_tick_) / 1000000);
    mean = ((double)value_snapshot) / times_snapshot;
  }

  snapshot_tick_ = now_tick;

  if (snapshot_value_ == NULL) {
    snapshot_value_ = new SimplerTimerSnapshot();
  }
  ((SimplerTimerSnapshot *)snapshot_value_)->setValue(mean, tps);
}

Histogram::Histogram(RandomGenerator &random) : UniformReservoir(random) {}

Histogram::Histogram(RandomGenerator &random, size_t size)
    : UniformReservoir(random, size) {}

Histogram::~Histogram() {

}

void Histogram::snapshot() {
  UniformReservoir::snapshot();
}

Timer::Timer(RandomGenerator &random)
    : UniformReservoir(random){
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  snapshot_tick_ = start_time.tv_sec * 1000000 + start_time.tv_usec;
  value_.store(0l);
}

Timer::Timer(RandomGenerator &random, size_t size)
    : UniformReservoir(random, size){
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  snapshot_tick_ = start_time.tv_sec * 1000000 + start_time.tv_usec;
  value_.store(0l);
}

Timer::~Timer() {
  if (snapshot_value_ == NULL) {
    delete snapshot_value_;
    snapshot_value_ = NULL;
  }
}

void Timer::update(double ms) {
  UniformReservoir::update(ms);
  value_.fetch_add(1l);
}

void Timer::snapshot() {
  if (snapshot_value_ == NULL) {
    snapshot_value_ = new TimerSnapshot();
  }
  TimerSnapshot *timer_snapshot = (TimerSnapshot *)snapshot_value_;

  struct timeval now;
  gettimeofday(&now, NULL);

  long now_tick = now.tv_sec * 1000000 + now.tv_usec;

  double tps =
      ((double)value_.exchange(0l) )/ ((now_tick - snapshot_tick_  ) / 1000000);
  snapshot_tick_ = now_tick;

  MUTEX_LOCK(&mutex);
  std::vector<double> output = data;
  MUTEX_UNLOCK(&mutex);

  timer_snapshot->set_collection(output);
  timer_snapshot->set_tps(tps);
}

TimerStat::TimerStat(SimpleTimer &other_st)
    : st_(other_st), start_tick_(0), end_tick_(0) {

  start();
}

TimerStat::~TimerStat() {
  if (end_tick_ == 0) {
    end();
  }

  st_.update((end_tick_ - start_tick_) / 1000);
}

void TimerStat::start() {
  struct timeval now;
  gettimeofday(&now, NULL);

  start_tick_ = now.tv_sec * 1000000 + now.tv_usec;
}

void TimerStat::end() {
  struct timeval now;
  gettimeofday(&now, NULL);

  end_tick_ = now.tv_sec * 1000000 + now.tv_usec;
}

} // namespace common