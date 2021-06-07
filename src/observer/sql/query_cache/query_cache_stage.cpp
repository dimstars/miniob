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

#include "query_cache_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"

using namespace common;

//! Constructor
QueryCacheStage::QueryCacheStage(const char *tag) : Stage(tag) {}

//! Destructor
QueryCacheStage::~QueryCacheStage() {}

//! Parse properties, instantiate a stage object
Stage *QueryCacheStage::makeStage(const std::string &tag) {
  QueryCacheStage *stage = new (std::nothrow) QueryCacheStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new QueryCacheStage failed");
    return nullptr;
  }
  stage->setProperties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool QueryCacheStage::setProperties() {
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
bool QueryCacheStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = nextStageList.begin();
  planCacheStage = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void QueryCacheStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void QueryCacheStage::handleEvent(StageEvent *event) {
  LOG_TRACE("Enter\n");

  // Add callback to update query cache
  /*
  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for SQLStageEvent");
    event->doneImmediate();
    return;
  }

  event->pushCallback(cb);
   */
  // do nothing here, pass the event to the next stage
  planCacheStage->handleEvent(event);

  LOG_TRACE("Exit\n");
  return;
}

void QueryCacheStage::callbackEvent(StageEvent *event,
                                    CallbackContext *context) {
  LOG_TRACE("Enter\n");

  // TODO update data to query cache here
  // event->doneImmediate();

  LOG_TRACE("Exit\n");
  return;
}
