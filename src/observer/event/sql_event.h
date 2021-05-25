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
// Created by Longda on 2021/4/14.
//

#ifndef __OBSERVER_SQL_EVENT_SQLEVENT_H__
#define __OBSERVER_SQL_EVENT_SQLEVENT_H__

#include "common/seda/stage_event.h"
#include <string>

class SessionEvent;

class SQLStageEvent : public common::StageEvent {
public:
  SQLStageEvent(SessionEvent *event, std::string &sql);
  virtual ~SQLStageEvent() noexcept;

  const std::string &get_sql() const {
    return sql_;
  }

  SessionEvent * session_event() const {
    return session_event_;
  }
private:
  SessionEvent *session_event_;
  std::string & sql_;
  // void *context_;
};

#endif //__SRC_OBSERVER_SQL_EVENT_SQLEVENT_H__
