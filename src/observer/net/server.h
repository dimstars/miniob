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
// Created by Longda on 2021/4/1.
//

#ifndef __OBSERVER_NET_SERVER_H__
#define __OBSERVER_NET_SERVER_H__

#include "common/defs.h"
#include "common/metrics/metrics.h"
#include "common/seda/stage.h"
#include "net/connection_context.h"
#include "net/server_param.h"

class Server {
public:
  Server(ServerParam input_server_param);
  ~Server();

public:
  static void init();
  static int send(ConnectionContext *client, const char *buf, int data_len);

public:
  int serve();
  void shutdown();

private:
  static void accept(int fd, short ev, void *arg);
  // close connection
  static void close_connection(ConnectionContext *client_context);
  static void recv(int fd, short ev, void *arg);

private:
  int set_non_block(int fd);
  int start();

private:
  bool started_;

  int server_socket_;
  struct event_base *event_base_;
  struct event *listen_ev_;

  ServerParam server_param_;

  static common::Stage *session_stage_;
  static common::SimpleTimer *read_socket_metric_;
  static common::SimpleTimer *write_socket_metric_;
};

#endif //__OBSERVER_NET_SERVER_H__
