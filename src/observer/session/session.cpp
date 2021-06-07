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
// Created by Wangyunlai on 2021/5/12.
//

#include "session/session.h"
#include "storage/trx/trx.h"

Session &Session::default_session() {
  static Session session;
  return session;
}

Session::Session(const Session &other) : current_db_(other.current_db_){
}

Session::~Session() {
  delete trx_;
  trx_ = nullptr;
}

const std::string &Session::get_current_db() const {
  return current_db_;
}
void Session::set_current_db(const std::string &dbname) {
  current_db_ = dbname;
}

void Session::set_trx_multi_operation_mode(bool multi_operation_mode) {
  trx_multi_operation_mode_ = multi_operation_mode;
}

bool Session::is_trx_multi_operation_mode() const {
  return trx_multi_operation_mode_;
}

Trx *Session::current_trx() {
  if (trx_ == nullptr) {
    trx_ = new Trx;
  }
  return trx_;
}
