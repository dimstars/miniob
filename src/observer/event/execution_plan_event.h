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
// Created by Wangyunlai on 2021/5/11.
//

#ifndef __OBSERVER_EVENT_EXECUTION_PLAN_EVENT_H__
#define __OBSERVER_EVENT_EXECUTION_PLAN_EVENT_H__

#include "common/seda/stage_event.h"
#include "handler/handler_defs.h"

class SQLStageEvent;

class ExecutionPlanEvent : public common::StageEvent {
public:
  ExecutionPlanEvent(SQLStageEvent *sql_event, sqlstr *sqls);
  virtual ~ExecutionPlanEvent();

  sqlstr * sqls() const {
    return sqls_;
  }

  SQLStageEvent * sql_event() const {
    return sql_event_;
  }
private:
  SQLStageEvent *      sql_event_;
  sqlstr *             sqls_;
};

#endif // __OBSERVER_EVENT_EXECUTION_PLAN_EVENT_H__