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

#ifndef __OBSERVER_SESSION_SESSIONSTAGE_H__
#define __OBSERVER_SESSION_SESSIONSTAGE_H__

#include "common/seda/stage.h"
#include "net/connection_context.h"
#include "common/metrics/metrics.h"

/**
 * seda::stage使用说明：
 * 这里利用seda的线程池与调度。stage是一个事件处理的几个阶段。
 * 目前包括session,parse,execution和storage
 * 每个stage使用handleEvent函数处理任务，并且使用StageEvent::pushCallback注册回调函数。
 * 这时当调用StageEvent::done(Immediate)时，就会调用该事件注册的回调函数。
 */
class SessionStage : public common::Stage {
public:
  ~SessionStage();
  static Stage *makeStage(const std::string &tag);

protected:
  // common function
  SessionStage(const char *tag);
  bool setProperties();

  bool initialize();
  void cleanup();
  void handleEvent(common::StageEvent *event);
  void callbackEvent(common::StageEvent *event,
                     common::CallbackContext *context);

protected:
  void handleInput(common::StageEvent *event);


  void handleRequest(common::StageEvent *event);

private:
  Stage *resolveStage;
  common::SimpleTimer *sqlMetric;
  static const std::string SQL_METRIC_TAG;

};

#endif //__OBSERVER_SESSION_SESSIONSTAGE_H__
