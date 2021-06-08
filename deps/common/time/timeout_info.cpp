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

#include "common/time/timeout_info.h"

#include <sys/time.h>
namespace common {

TimeoutInfo::TimeoutInfo(time_t deadLine)
  : deadline_(deadLine), is_timed_out_(false), ref_cnt_(0) {
  MUTEX_INIT(&mutex_, NULL);
}

TimeoutInfo::~TimeoutInfo() {
  // unlock mutex_ as we locked it before 'delete this'
  MUTEX_UNLOCK(&mutex_);

  MUTEX_DESTROY(&mutex_);
}

void TimeoutInfo::attach() {
  MUTEX_LOCK(&mutex_);
  ref_cnt_++;
  MUTEX_UNLOCK(&mutex_);
}

void TimeoutInfo::detach() {
  MUTEX_LOCK(&mutex_);
  if (0 == --ref_cnt_) {
    delete this;
    return;
  }
  MUTEX_UNLOCK(&mutex_);
}

bool TimeoutInfo::has_timed_out() {
  MUTEX_LOCK(&mutex_);
  bool ret = is_timed_out_;
  if (!is_timed_out_) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    ret = is_timed_out_ = (tv.tv_sec >= deadline_);
  }
  MUTEX_UNLOCK(&mutex_);

  return ret;
}

} //namespace common