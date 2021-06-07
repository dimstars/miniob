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

// __CR__

/*
 *  Created on: Mar 11, 2012
 *      Author: Longda Feng
 */


#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#include "init.h"
#include "common/os/process.h"
#include "common/os/signal.h"
#include "net/server.h"
#include "net/server_param.h"

using namespace common;

#define NET "NET"

static Server *g_server = nullptr;

void usage() {
  // TODO ADD USAGE
  std::cout << "Useage " << std::endl;
}

void parse_parameter(int argc, char **argv) {
  std::string process_name = get_process_name(argv[0]);

  ProcessParam *process_param = the_process_param();

  process_param->init_default(process_name);

  // Process args
  int opt;
  extern char *optarg;
  while ((opt = getopt(argc, argv, "ds:f:o:e:h")) > 0) {
    switch (opt) {
    case 'f':
      process_param->set_conf(optarg);
      break;
    case 'o':
      process_param->set_std_out(optarg);
      break;
    case 'e':
      process_param->set_std_err(optarg);
      break;
    case 'd':
      process_param->set_demon(true);
      break;
    case 'h':
    default:
      usage();
      return;
    }
  }
}

Server *init_server() {
  std::map<std::string, std::string> net_section =
      g_properties()->get(NET);

  long listen_addr = INADDR_ANY;
  long max_connection_num = MAX_CONNECTION_NUM_DEFAULT;
  int port = PORT_DEFAULT;

  std::map<std::string, std::string>::iterator it = net_section.find(CLIENT_ADDRESS);
  if (it != net_section.end()) {
    std::string str = it->second;
    str_to_val(str, listen_addr);
  }

  it = net_section.find(MAX_CONNECTION_NUM);
  if (it != net_section.end()) {
    std::string str = it->second;
    str_to_val(str, max_connection_num);
  }

  it = net_section.find(PORT);
  if (it != net_section.end()) {
    std::string str = it->second;
    str_to_val(str, port);
  }

  ServerParam server_param;
  server_param.listen_addr = listen_addr;
  server_param.max_connection_num = max_connection_num;
  server_param.port = port;

  Server *server = new Server(server_param);
  return server;
}

void quit_signal_handle(int signum) {
  LOG_INFO("Receive signal: %d", signum);
  if (g_server) {
    g_server->shutdown();
    delete g_server;
    g_server = nullptr;
  }
}

int main(int argc, char **argv) {
  setSignalHandler(quit_signal_handle);

  parse_parameter(argc, argv);

  int rc = STATUS_SUCCESS;
  rc = init(the_process_param());
  if (rc) {
    std::cerr << "Shutdown due to failed to init!" << std::endl;
    cleanup();
    return rc;
  }

  g_server = init_server();
  Server::init();
  g_server->serve();

  LOG_INFO("Server stopped");

  cleanup();
}
