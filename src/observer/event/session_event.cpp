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

#include "session_event.h"

SessionEvent::SessionEvent(ConnectionContext *client) : client_(client) {
}

SessionEvent::~SessionEvent() {

}

ConnectionContext *SessionEvent::get_client() const { return client_; }

const char *SessionEvent::get_response() const { 
  return response_.c_str(); 
}

void SessionEvent::set_response(const char *response) {
  set_response(response, strlen(response));
}

void SessionEvent::set_response(const char *response, int len) {
  response_.assign(response, len);
}

void SessionEvent::set_response(std::string &&response) {
  response_ = std::move(response);
}

int SessionEvent::get_response_len() const { return response_.size(); }

char *SessionEvent::get_request_buf() { return client_->buf; }

int SessionEvent::get_request_buf_len() { return SOCKET_BUFFER_SIZE; }