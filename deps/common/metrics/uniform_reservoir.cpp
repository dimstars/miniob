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

#include "common/metrics/uniform_reservoir.h"

#include <stdint.h>

#include "common/lang/mutex.h"
#include "common/metrics/histogram_snapshot.h"

namespace common {

#define DEFAULT_SIZE 1023

UniformReservoir::UniformReservoir(RandomGenerator &random)
    : Reservoir(random), counter(0) {
  pthread_mutexattr_t mutexatr;
  pthread_mutexattr_init(&mutexatr);
  pthread_mutexattr_settype(&mutexatr, PTHREAD_MUTEX_RECURSIVE);

  MUTEX_INIT(&mutex, &mutexatr);

  init(DEFAULT_SIZE);
}

UniformReservoir::UniformReservoir(RandomGenerator &random, size_t size)
    : Reservoir(random), counter(0) {

  pthread_mutexattr_t mutexatr;
  pthread_mutexattr_init(&mutexatr);
  pthread_mutexattr_settype(&mutexatr, PTHREAD_MUTEX_RECURSIVE);

  MUTEX_INIT(&mutex, &mutexatr);
  init(size);
}

UniformReservoir::~UniformReservoir() {
  if (snapshot_value_ == NULL) {
    delete snapshot_value_;
    snapshot_value_ = NULL;
  }
}

void UniformReservoir::init(size_t size) {
  MUTEX_LOCK(&mutex);
  counter = 0;
  data.resize(size);
  MUTEX_UNLOCK(&mutex);
}

size_t UniformReservoir::size() {
  MUTEX_LOCK(&mutex);
  size_t size = (counter < data.size()) ? counter : data.size();
  MUTEX_UNLOCK(&mutex);
  return size;
}

int UniformReservoir::get_count() {
  MUTEX_LOCK(&mutex);
  int ret = counter;
  MUTEX_UNLOCK(&mutex);
  return ret;
}

void UniformReservoir::update(double value) {
  MUTEX_LOCK(&mutex);
  double count = ++counter;

  if (count < data.size()) {
    data[count] = (value);
  } else {
    double rcount = next(data.size());
    data[rcount] = (value);
  }

  MUTEX_UNLOCK(&mutex);
}

void UniformReservoir::snapshot() {
  MUTEX_LOCK(&mutex);
  std::vector<double> output = data;
  MUTEX_UNLOCK(&mutex);

  if (snapshot_value_ == NULL) {
    snapshot_value_ = new HistogramSnapShot();
  }
  ((HistogramSnapShot *)snapshot_value_)->set_collection(output);
}

void UniformReservoir::reset() {

  MUTEX_LOCK(&mutex);
  counter = 0;
  data.clear();

  // TODO clear snapshot
  MUTEX_UNLOCK(&mutex);
}

} // namespace common