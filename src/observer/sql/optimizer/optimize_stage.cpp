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

#include "optimize_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"

using namespace common;

//! Constructor
OptimizeStage::OptimizeStage(const char *tag) : Stage(tag) {}

//! Destructor
OptimizeStage::~OptimizeStage() {}

//! Parse properties, instantiate a stage object
Stage *OptimizeStage::make_stage(const std::string &tag) {
  OptimizeStage *stage = new (std::nothrow) OptimizeStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new OptimizeStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool OptimizeStage::set_properties() {
  //  std::string stageNameStr(stage_name_);
  //  std::map<std::string, std::string> section = g_properties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool OptimizeStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  executeStage = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void OptimizeStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void OptimizeStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  // TODO optimize sql plan, here just pass the event to the next stage
  executeStage->handle_event(event);

  LOG_TRACE("Exit\n");
  return;
}

void OptimizeStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  LOG_TRACE("Exit\n");
  return;
}
