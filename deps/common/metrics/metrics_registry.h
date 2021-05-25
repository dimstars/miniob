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

#ifndef __COMMON_METRICS_METRICS_REGISTRY_H__
#define __COMMON_METRICS_METRICS_REGISTRY_H__

#include <string>
#include <map>
#include <list>

#include "common/metrics/metric.h"
#include "common/metrics/reporter.h"

namespace common {

class MetricsRegistry {
public:
  MetricsRegistry() {};
  virtual ~MetricsRegistry(){};

  void registerMetric(const std::string &tag, Metric *metric);
  void unregister(const std::string &tag);

  void snapshot();

  void report();

  void addReporter(Reporter *reporter) {
    reporters.push_back(reporter);
  }


protected:
  std::map<std::string, Metric *> metrics;
  std::list<Reporter *> reporters;


};

MetricsRegistry& theGlobalMetricsRegistry();
}//namespace common
#endif //__COMMON_METRICS_METRICS_REGISTRY_H__
