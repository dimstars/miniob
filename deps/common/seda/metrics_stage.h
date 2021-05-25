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
// Created by Longda on 2021/4/13.
//

#ifndef __COMMON_SEDA_METRICS_STAGE_H__
#define __COMMON_SEDA_METRICS_STAGE_H__

#include "common/seda/stage.h"

namespace common {

class MetricsStage : public Stage {
public:
  ~MetricsStage();
  static Stage *makeStage(const std::string &tag);

protected:
  // common function
  MetricsStage(const char *tag);
  bool setProperties();

  bool initialize();
  void cleanup();
  void handleEvent(StageEvent *event);
  void callbackEvent(StageEvent *event, CallbackContext *context);

protected:
private:
  Stage *timerStage;
  //report metrics every @metricReportInterval seconds
  int  metricReportInterval;
};
} // namespace common
#endif //__COMMON_SEDA_METRICS_STAGE_H__
