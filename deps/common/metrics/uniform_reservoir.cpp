/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
OceanBase is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

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