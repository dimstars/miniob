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
  struct timeval startTime;
  gettimeofday(&startTime, NULL);

  snapshotTick = startTime.tv_sec * 1000000 + startTime.tv_usec;
  value.store(0l);
}

Meter::~Meter() {
  if (snapshotValue != NULL) {
    delete snapshotValue;
    snapshotValue = NULL;
  }
}

void Meter::inc(long increase) { value.fetch_add(increase); }
void Meter::inc() { inc(1l); }

void Meter::snapshot() {
  // TODO lock here

  struct timeval now;
  gettimeofday(&now, NULL);

  long nowTick = now.tv_sec * 1000000 + now.tv_usec;

  double tempValue =
      ((double)value.exchange(0l)) / ((nowTick - snapshotTick ) / 1000000);
  snapshotTick = nowTick;

  if (snapshotValue == NULL) {
    snapshotValue = new SnapshotBasic<double>();
  }
  ((SnapshotBasic<double> *)snapshotValue)->setValue(tempValue);
}

SimpleTimer::~SimpleTimer() {
  if (snapshotValue != NULL) {
    delete snapshotValue;
    snapshotValue = NULL;
  }
}

void SimpleTimer::inc(long increase) {
  value.fetch_add(increase);
  times.fetch_add(1);
}

void SimpleTimer::update(long one) { inc(one); }

void SimpleTimer::snapshot() {

  // TODO lock here
  struct timeval now;
  gettimeofday(&now, NULL);

  long nowTick = now.tv_sec * 1000000 + now.tv_usec;

  long valueSnapshot = value.exchange(0l);
  long timesSnapshot = times.exchange(0l);

  double tps = 0;
  double mean = 0;

  if (timesSnapshot > 0) {
    tps = ((double)timesSnapshot )/ ((nowTick - snapshotTick) / 1000000);
    mean = ((double)valueSnapshot) / timesSnapshot;
  }

  snapshotTick = nowTick;

  if (snapshotValue == NULL) {
    snapshotValue = new SimplerTimerSnapshot();
  }
  ((SimplerTimerSnapshot *)snapshotValue)->setValue(mean, tps);
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
  struct timeval startTime;
  gettimeofday(&startTime, NULL);

  snapshotTick = startTime.tv_sec * 1000000 + startTime.tv_usec;
  value.store(0l);
}

Timer::Timer(RandomGenerator &random, size_t size)
    : UniformReservoir(random, size){
  struct timeval startTime;
  gettimeofday(&startTime, NULL);

  snapshotTick = startTime.tv_sec * 1000000 + startTime.tv_usec;
  value.store(0l);
}

Timer::~Timer() {
  if (snapshotValue == NULL) {
    delete snapshotValue;
    snapshotValue = NULL;
  }
}

void Timer::update(double ms) {
  UniformReservoir::update(ms);
  value.fetch_add(1l);
}

void Timer::snapshot() {
  if (snapshotValue == NULL) {
    snapshotValue = new TimerSnapshot();
  }
  TimerSnapshot *timerSnapshot = (TimerSnapshot *)snapshotValue;

  struct timeval now;
  gettimeofday(&now, NULL);

  long nowTick = now.tv_sec * 1000000 + now.tv_usec;

  double tps =
      ((double)value.exchange(0l) )/ ((nowTick - snapshotTick  ) / 1000000);
  snapshotTick = nowTick;

  MUTEX_LOCK(&mutex);
  std::vector<double> output = data;
  MUTEX_UNLOCK(&mutex);

  timerSnapshot->setCollection(output);
  timerSnapshot->setTps(tps);
}

TimerStat::TimerStat(SimpleTimer &otherST)
    : st(otherST), startTick(0), endTick(0) {

  start();
}

TimerStat::~TimerStat() {
  if (endTick == 0) {
    end();
  }

  st.update((endTick - startTick) / 1000);
}

void TimerStat::start() {
  struct timeval now;
  gettimeofday(&now, NULL);

  startTick = now.tv_sec * 1000000 + now.tv_usec;
}

void TimerStat::end() {
  struct timeval now;
  gettimeofday(&now, NULL);

  endTick = now.tv_sec * 1000000 + now.tv_usec;
}

} // namespace common