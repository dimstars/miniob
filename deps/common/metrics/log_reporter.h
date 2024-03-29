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

#ifndef __COMMON_METRICS_LOG_REPORTER_H__
#define __COMMON_METRICS_LOG_REPORTER_H__

#include "common/metrics/reporter.h"

namespace common {


class LogReporter : public Reporter {
public:
  void report(const std::string &tag, Metric *metric);
};

LogReporter* get_log_reporter();
} //namespace common
#endif //__COMMON_METRICS_LOG_REPORTER_H__
