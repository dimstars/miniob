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

#include "common/metrics/console_reporter.h"

#include <iostream>
#include <string>

#include "common/metrics/metric.h"

namespace common {

ConsoleReporter *get_console_reporter() {
  static ConsoleReporter *instance = new ConsoleReporter();

  return instance;
}

void ConsoleReporter::report(const std::string &tag, Metric *metric) {
  Snapshot *snapshot = metric->get_snapshot();

  if (snapshot != NULL) {
    printf("%s:%s\n", tag.c_str(), snapshot->to_string().c_str());
  } else {
    printf("There is no snapshot of %s metrics.", tag.c_str());
  }
}

} // namespace common