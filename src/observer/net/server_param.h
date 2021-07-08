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

#ifndef __SRC_OBSERVER_NET_SERVER_PARAM_H__
#define __SRC_OBSERVER_NET_SERVER_PARAM_H__

#include <string>

class ServerParam {
public:
  ServerParam();

  ServerParam(const ServerParam &other) = default;
  ~ServerParam() = default;

public:
  // accpet client's address, default is INADDR_ANY, means accept every address
  long listen_addr;

  int max_connection_num;
  // server listing port
  int port;

  std::string unix_socket_path;

  // 如果使用标准输入输出作为通信条件，就不再监听端口
  bool use_unix_socket = false;
};

#endif //__SRC_OBSERVER_NET_SERVER_PARAM_H__
