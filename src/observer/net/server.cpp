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
// Created by Longda on 2021
//

#include "net/server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/lang/mutex.h"
#include "common/log/log.h"
#include "common/seda/seda_config.h"
#include "event/session_event.h"
#include "session/session.h"
#include "ini_setting.h"
#include <common/metrics/metrics_registry.h>

using namespace common;
static const std::string READ_SOCKET_METRIC_TAG = "SessionStage.readsocket";
static const std::string WRITE_SOCKET_METRIC_TAG = "SessionStage.writesocket";

Stage *Server::session_stage_ = nullptr;
common::SimpleTimer *Server::read_socket_metric_ = nullptr;
common::SimpleTimer *Server::write_socket_metric_ = nullptr;

ServerParam::ServerParam() {
  listen_addr = INADDR_ANY;
  max_connection_num = MAX_CONNECTION_NUM_DEFAULT;
  port = PORT_DEFAULT;
}

ServerParam::ServerParam(const ServerParam &other) {
  if (&other != this) {
    this->listen_addr = other.listen_addr;
    this->max_connection_num = other.max_connection_num;
    this->port = other.port;
  }
}

Server::Server(ServerParam input_server_param) : server_param_(input_server_param) {
  started_ = false;
  server_socket_ = 0;
  event_base_ = nullptr;
  listen_ev_ = nullptr;
}

Server::~Server() {
  if (started_) {
    shutdown();
  }
}

void Server::init(){
  session_stage_ = theSedaConfig()->getStage(SESSION_STAGE_NAME);

  MetricsRegistry &metricsRegistry = g_metrics_registry();
  if (Server::read_socket_metric_ == nullptr) {
    Server::read_socket_metric_ = new SimpleTimer();
    metricsRegistry.register_metric(READ_SOCKET_METRIC_TAG, Server::read_socket_metric_);
  }

  if (Server::write_socket_metric_ == nullptr) {
    Server::write_socket_metric_ = new SimpleTimer();
    metricsRegistry.register_metric(WRITE_SOCKET_METRIC_TAG, Server::write_socket_metric_);
  }  
}

int Server::set_non_block(int fd) {

  int flags = fcntl(fd, F_GETFL);
  if (flags == -1) {
    LOG_INFO("Failed to get flags of fd :%d. ", fd);
    return -1;
  }

  flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  if (flags == -1) {
    LOG_INFO("Failed to set non-block flags of fd :%d. ", fd);
    return -1;
  }
  return 0;
}

void Server::close_connection(ConnectionContext *client_context) {
  LOG_INFO("Close connection of %s.", client_context->addr);
  event_del(&client_context->read_event);
  ::close(client_context->fd);
  delete client_context->session;
  client_context->session = nullptr;
  delete client_context;
}

void Server::recv(int fd, short ev, void *arg) {
  ConnectionContext *client = (ConnectionContext *)arg;
  //Server::send(sev->getClient(), sev->getRequestBuf(), strlen(sev->getRequestBuf()));

  TimerStat timer_stat(*read_socket_metric_);
  MUTEX_LOCK(&client->mutex);
  int len = ::read(client->fd, client->buf, sizeof(client->buf));
  MUTEX_UNLOCK(&client->mutex);
  timer_stat.end();
  if (len == 0) {
    LOG_INFO("The peer has been closed %s\n", client->addr);
    close_connection(client);
    return;
  } else if (len < 0) {
    LOG_ERROR("Failed to read socket of %s, %s\n", client->addr,
              strerror(errno));
    close_connection(client);
    return;
  }

  SessionEvent *sev = new SessionEvent(client);
  session_stage_->addEvent(sev);
}

int Server::send(ConnectionContext *client, const char *buf, int data_len) {
  if (buf == nullptr || data_len == 0) {
    return 0;
  }

  TimerStat writeStat(*write_socket_metric_);

  MUTEX_LOCK(&client->mutex);

  int wlen = 0;
  for (int i = 0; i < 3 && wlen < data_len; i++) {
    int len = write(client->fd, buf, data_len - wlen);
    if (len < 0) {
      LOG_ERROR("Failed to send data back to client\n");
      MUTEX_UNLOCK(&client->mutex);

      close_connection(client);
      return -STATUS_FAILED_NETWORK;
    }
    wlen += len;
  }
  if (wlen < data_len) {
    LOG_WARN("Not all data has been send back to client");
  }

  MUTEX_UNLOCK(&client->mutex);
  return 0;
}

void Server::accept(int fd, short ev, void *arg) {
  Server *instance = (Server *)arg;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  int ret = 0;

  int client_fd = ::accept(fd, (struct sockaddr *)&addr, &addrlen);
  if (client_fd < 0) {
    LOG_ERROR("Failed to accept client's connection, %s", strerror(errno));
    return;
  }

  char ip_addr[24];
  if (inet_ntop(AF_INET, &addr.sin_addr, ip_addr, sizeof(ip_addr)) == nullptr) {
    LOG_ERROR("Failed to get ip address of client, %s", strerror(errno));
    ::close(client_fd);
    return;
  }
  std::stringstream address;
  address << ip_addr << ":" << addr.sin_port;
  std::string addr_str = address.str();

  ret = instance->set_non_block(client_fd);
  if (ret < 0) {
    LOG_ERROR("Failed to set socket of %s as non blocking, %s", addr_str.c_str(),
              strerror(errno));
    ::close(client_fd);
    return;
  }

  int yes = 1;
  ret = setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
  if (ret < 0) {
    LOG_ERROR("Failed to set socket of %s option as : TCP_NODELAY %s\n",
              addr_str.c_str(), strerror(errno));
    ::close(client_fd);
    return;
  }

  ConnectionContext *client_context = new ConnectionContext();
  memset(client_context, 0, sizeof(ConnectionContext));
  client_context->fd = client_fd;
  strncpy(client_context->addr, addr_str.c_str(), sizeof(client_context->addr));
  pthread_mutex_init(&client_context->mutex, nullptr);

  event_set(&client_context->read_event, client_context->fd, EV_READ | EV_PERSIST,
            recv, client_context);

  ret = event_base_set(instance->event_base_, &client_context->read_event);
  if (ret < 0) {
    LOG_ERROR(
            "Failed to do event_base_set for read event of %s into libevent, %s",
            client_context->addr, strerror(errno));
    delete client_context;
    ::close(instance->server_socket_);
    return;
  }

  ret = event_add(&client_context->read_event, nullptr);
  if (ret < 0) {
    LOG_ERROR("Failed to event_add for read event of %s into libevent, %s",
              client_context->addr, strerror(errno));
    delete client_context;
    ::close(instance->server_socket_);
    return;
  }

  client_context->session = new Session(Session::default_session());
  LOG_INFO("Accepted connection from %s\n", client_context->addr);
}

int Server::start() {
  int ret = 0;
  struct sockaddr_in sa;

  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    LOG_ERROR("socket(): can not create server socket: %s.", strerror(errno));
    return -1;
  }

  int yes = 1;
  ret = setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  if (ret < 0) {
    LOG_ERROR("Failed to set socket option of reuse address: %s.",
              strerror(errno));
    ::close(server_socket_);
    return -1;
  }

  ret = set_non_block(server_socket_);
  if (ret < 0) {
    LOG_ERROR("Failed to set socket option non-blocking:%s. ", strerror(errno));
    ::close(server_socket_);
    return -1;
  }

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(server_param_.port);
  sa.sin_addr.s_addr = htonl(server_param_.listen_addr);

  ret = bind(server_socket_, (struct sockaddr *)&sa, sizeof(sa));
  if (ret < 0) {
    LOG_ERROR("bind(): can not bind server socket, %s", strerror(errno));
    ::close(server_socket_);
    return -1;
  }

  ret = listen(server_socket_, server_param_.max_connection_num);
  if (ret < 0) {
    LOG_ERROR("listen(): can not listen server socket, %s", strerror(errno));
    ::close(server_socket_);
    return -1;
  }
  LOG_INFO("Listen on port %d", server_param_.port);

  listen_ev_ = event_new(event_base_, server_socket_, EV_READ | EV_PERSIST, accept, this);
  if (listen_ev_ == nullptr) {
    LOG_ERROR("Failed to create listen event, %s.", strerror(errno));
    ::close(server_socket_);
    return -1;
  }

  ret = event_add(listen_ev_, nullptr);
  if (ret < 0) {
    LOG_ERROR("event_add(): can not add accept event into libevent, %s",
              strerror(errno));
    ::close(server_socket_);
    return -1;
  }

  started_ = true;
  LOG_INFO("Observer start success");
  return 0;
}

int Server::serve() {
  event_base_ = event_base_new();
  if (event_base_ == nullptr) {
    LOG_ERROR("Failed to create event base, %s.", strerror(errno));
    exit(-1);
  }

  int retval = start();
  if (retval == -1) {
    LOG_PANIC("Failed to start network");
    exit(-1);
  }

  event_base_dispatch(event_base_);

  return 0;
}

void Server::shutdown() {
  LOG_INFO("Server shutting down");

  // cleanup
  struct timeval exit_time;
  gettimeofday(&exit_time, nullptr);
  exit_time.tv_sec += 10;
  event_base_loopexit(event_base_, &exit_time);

  event_del(listen_ev_);
  event_free(listen_ev_);
  listen_ev_ = nullptr;
  event_base_free(event_base_);
  event_base_ = nullptr;

  started_ = false;
  LOG_INFO("Server quit");
}
