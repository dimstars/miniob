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

#include <string.h>
#include <string>

#include "session_stage.h"

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

//! Constructor
SessionStage::SessionStage(const char *tag)
    : Stage(tag), resolveStage(nullptr), sqlMetric(nullptr) {}

//! Destructor
SessionStage::~SessionStage() {}

//! Parse properties, instantiate a stage object
Stage *SessionStage::makeStage(const std::string &tag) {
  SessionStage *stage = new (std::nothrow) SessionStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecutorStage failed");
    return nullptr;
  }
  stage->setProperties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool SessionStage::setProperties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = g_properties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool SessionStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = nextStageList.begin();
  resolveStage = *(stgp++);

  MetricsRegistry &metricsRegistry = theGlobalMetricsRegistry();
  sqlMetric = new SimpleTimer();
  metricsRegistry.registerMetric(SQL_METRIC_TAG, sqlMetric);
  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void SessionStage::cleanup() {
  LOG_TRACE("Enter");

  MetricsRegistry &metricsRegistry = theGlobalMetricsRegistry();
  if (sqlMetric != nullptr) {
    metricsRegistry.unregister(SQL_METRIC_TAG);
    delete sqlMetric;
    sqlMetric = nullptr;
  }

  LOG_TRACE("Exit");
}

void SessionStage::handleEvent(StageEvent *event) {
  LOG_TRACE("Enter\n");

  // right now, we just support only one event.
  handleRequest(event);

  LOG_TRACE("Exit\n");
  return;
}

void SessionStage::callbackEvent(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  SessionEvent *sev = dynamic_cast<SessionEvent *>(event);

  const char *response = sev->get_response();
  int len = sev->get_response_len();
  if (len <= 0) {
    response = "No data\n";
    len = strlen(response);
  }
  Server::send(sev->get_client(), response, len);

  sev->done();
  LOG_TRACE("Exit\n");
  return;
}

void SessionStage::handleRequest(StageEvent *event) {

  SessionEvent *sev = dynamic_cast<SessionEvent *>(event);

  TimerStat sqlStat(*sqlMetric);
  std::string sql = sev->get_request_buf();
  if (common::is_blank(sql.c_str())) {
    sev->doneImmediate();
    return;
  }

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for SessionEvent");

    sev->done();
    return;
  }

  sev->pushCallback(cb);

  SQLStageEvent *sql_event = new SQLStageEvent(sev, sql);
  resolveStage->handleEvent(sql_event);

  // TODO it will write data directly
}
