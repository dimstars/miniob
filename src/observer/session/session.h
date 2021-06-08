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

#ifndef __OBSERVER_SESSION_SESSION_H__
#define __OBSERVER_SESSION_SESSION_H__

#include <string>

class Trx;

class Session {
public:
  // static Session &current();
  static Session &default_session();

public:
  Session() = default;
  ~Session();

  Session(const Session & other);
  void operator =(Session &) = delete;

  const std::string &get_current_db() const;
  void set_current_db(const std::string &dbname);

  void set_trx_multi_operation_mode(bool multi_operation_mode);
  bool is_trx_multi_operation_mode() const;

  Trx * current_trx();

private:
  std::string  current_db_;
  Trx         *trx_ = nullptr;
  bool         trx_multi_operation_mode_ = false; // 当前事务的模式，是否多语句模式. 单语句模式自动提交
};

#endif // __OBSERVER_SESSION_SESSION_H__