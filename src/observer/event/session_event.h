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

#ifndef __OBSERVER_SESSION_SESSIONEVENT_H__
#define __OBSERVER_SESSION_SESSIONEVENT_H__

#include <string.h>
#include <string>

#include "common/seda/stage_event.h"
#include "net/connection_context.h"

// TODO refactor
class SessionEvent : public common::StageEvent {
public:
  SessionEvent(ConnectionContext *client);
  virtual ~SessionEvent();

  ConnectionContext *get_client() const;

  const char *get_response() const;
  void set_response(const char *response);
  void set_response(const char *response, int len);
  void set_response(std::string &&response);
  int get_response_len() const;
  char *get_request_buf();
  int get_request_buf_len();

private:
  ConnectionContext *client;

  std::string response_;
  int responseLen;
};

#endif //__OBSERVER_SESSION_SESSIONEVENT_H__
