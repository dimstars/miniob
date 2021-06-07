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

#include "resolve_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "event/sql_event.h"

using namespace common;

//! Constructor
ResolveStage::ResolveStage(const char *tag) : Stage(tag) {}

//! Destructor
ResolveStage::~ResolveStage() {}

//! Parse properties, instantiate a stage object
Stage *ResolveStage::makeStage(const std::string &tag) {
  ResolveStage *stage = new (std::nothrow) ResolveStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ResolveStage failed");
    return nullptr;
  }
  stage->setProperties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ResolveStage::setProperties() {
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
bool ResolveStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = nextStageList.begin();
  queryCacheStage = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ResolveStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ResolveStage::handleEvent(StageEvent *event) {
  LOG_TRACE("Enter\n");

  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);

  // do nothing here
  queryCacheStage->handleEvent(sql_event);

  LOG_TRACE("Exit\n");
  return;
}

void ResolveStage::callbackEvent(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  LOG_TRACE("Exit\n");
  return;
}
