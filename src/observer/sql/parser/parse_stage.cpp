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

#include "parse_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "sql/parser/parse.h"
#include "event/execution_plan_event.h"

using namespace common;

//! Constructor
ParseStage::ParseStage(const char *tag) : Stage(tag) {}

//! Destructor
ParseStage::~ParseStage() {}

//! Parse properties, instantiate a stage object
Stage *ParseStage::make_stage(const std::string &tag) {
  ParseStage *stage = new (std::nothrow) ParseStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ParseStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ParseStage::set_properties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ParseStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  optimize_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ParseStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ParseStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  StageEvent *new_event = handle_request(event);
  if (nullptr == new_event) {
    event->done_immediate();
    return;
  }

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for SQLStageEvent");
    event->done_immediate();
    return;
  }
  event->push_callback(cb);
  optimize_stage_->handle_event(new_event);

  LOG_TRACE("Exit\n");
  return;
}

void ParseStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  sql_event->session_event()->done_immediate();
  LOG_TRACE("Exit\n");
  return;
}

StageEvent *ParseStage::handle_request(StageEvent *event) {
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  const std::string &sql = sql_event->get_sql();
  
  Query *result = query_create();
  if (nullptr == result) {
    LOG_ERROR("Failed to create query.");
    return nullptr;
  }

  RC ret = parse(sql.c_str(), result);
  if (ret != RC::SUCCESS) {
    // TODO set error information to event
    const char *error = result->sstr.errors != nullptr ? result->sstr.errors : "Unknown error";
    char response[256];
    snprintf(response, sizeof(response), "Failed to parse sql: %s, error msg: %s\n", sql.c_str(), error);
    sql_event->session_event()->set_response(response);
    query_destroy(result);
    return nullptr;
  }

  return new ExecutionPlanEvent(sql_event, result);
}
