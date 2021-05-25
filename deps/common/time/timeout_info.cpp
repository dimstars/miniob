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
// Created by Longda on 2010
//

#include <sys/time.h>

#include "common/time/timeout_info.h"
namespace common {

TimeoutInfo::TimeoutInfo(time_t deadLine)
  : deadline(deadLine), isTimedOut(false), refCnt(0) {
  MUTEX_INIT(&mutex, NULL);
}

TimeoutInfo::~TimeoutInfo() {
  // unlock mutex as we locked it before 'delete this'
  MUTEX_UNLOCK(&mutex);

  MUTEX_DESTROY(&mutex);
}

void TimeoutInfo::attach() {
  MUTEX_LOCK(&mutex);
  refCnt++;
  MUTEX_UNLOCK(&mutex);
}

void TimeoutInfo::detach() {
  MUTEX_LOCK(&mutex);
  if (0 == --refCnt) {
    delete this;
    return;
  }
  MUTEX_UNLOCK(&mutex);
}

bool TimeoutInfo::hasTimedOut() {
  MUTEX_LOCK(&mutex);
  bool ret = isTimedOut;
  if (!isTimedOut) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    ret = isTimedOut = (tv.tv_sec >= deadline);
  }
  MUTEX_UNLOCK(&mutex);

  return ret;
}

} //namespace common