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

#include "event/execution_plan_event.h"
#include "event/sql_event.h"

ExecutionPlanEvent::ExecutionPlanEvent(SQLStageEvent *sql_event, sqlstr *sqls) : sql_event_(sql_event), sqls_(sqls) {
}
ExecutionPlanEvent::~ExecutionPlanEvent() {
  if (sql_event_) {
    sql_event_->doneImmediate();
    sql_event_ = nullptr;
  }
}

