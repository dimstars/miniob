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

#include "session_stage.h"

#include <string.h>
#include <string>

#include "common/conf/ini.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"

#include "common/lang/mutex.h"
#include "common/metrics/metrics_registry.h"
#include "common/seda/callback.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "net/server.h"
#include "session/session.h"

using namespace common;

const std::string SessionStage::SQL_METRIC_TAG = "SessionStage.sql";

// Constructor
SessionStage::SessionStage(const char *tag)
    : Stage(tag), resolve_stage_(nullptr), sql_metric_(nullptr) {}

// Destructor
SessionStage::~SessionStage() {}

// Parse properties, instantiate a stage object
Stage *SessionStage::make_stage(const std::string &tag) {
  SessionStage *stage = new (std::nothrow) SessionStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecutorStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

// Set properties for this object set in stage specific properties
bool SessionStage::set_properties() {
  //  std::string stageNameStr(stage_name_);
  //  std::map<std::string, std::string> section = g_properties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

// Initialize stage params and validate outputs
bool SessionStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  resolve_stage_ = *(stgp++);

  MetricsRegistry &metricsRegistry = get_metrics_registry();
  sql_metric_ = new SimpleTimer();
  metricsRegistry.register_metric(SQL_METRIC_TAG, sql_metric_);
  LOG_TRACE("Exit");
  return true;
}

// Cleanup after disconnection
void SessionStage::cleanup() {
  LOG_TRACE("Enter");

  MetricsRegistry &metricsRegistry = get_metrics_registry();
  if (sql_metric_ != nullptr) {
    metricsRegistry.unregister(SQL_METRIC_TAG);
    delete sql_metric_;
    sql_metric_ = nullptr;
  }

  LOG_TRACE("Exit");
}

void SessionStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  // right now, we just support only one event.
  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void SessionStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  SessionEvent *sev = dynamic_cast<SessionEvent *>(event);
  // TODO 优化超长字段的返回
  const char *response = sev->get_response();
  int len = sev->get_response_len();
  if (len <= 0) {
    response = "No data\n";
    len = strlen(response) + 1;
  }
  Server::send(sev->get_client(), response, len);
	if ('\0' != response[len - 1]) {
		// 这里强制性的给发送一个消息终结符，如果需要发送多条消息，需要调整
		char end = 0;
		Server::send(sev->get_client(), &end, 1);
	}

  // sev->done();
  LOG_TRACE("Exit\n");
  return;
}

void SessionStage::handle_request(StageEvent *event) {

  SessionEvent *sev = dynamic_cast<SessionEvent *>(event);

  TimerStat sqlStat(*sql_metric_);
  std::string sql = sev->get_request_buf();
  if (common::is_blank(sql.c_str())) {
    sev->done_immediate();
    return;
  }

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for SessionEvent");

    sev->done_immediate();
    return;
  }

  sev->push_callback(cb);

  SQLStageEvent *sql_event = new SQLStageEvent(sev, sql);
  resolve_stage_->handle_event(sql_event);
}
