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

#include "common/seda/metrics_stage.h"

#include <string.h>
#include <string>

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/metrics/metrics_registry.h"
#include "common/seda/metrics_report_event.h"
#include "common/seda/timer_stage.h"
#include "common/time/datetime.h"
#include "common/seda/seda_defs.h"

using namespace common;

MetricsRegistry &get_metric_registry() {
  static MetricsRegistry metrics_registry;

  return metrics_registry;
}

// Constructor
MetricsStage::MetricsStage(const char *tag) : Stage(tag) {}

// Destructor
MetricsStage::~MetricsStage() {}

// Parse properties, instantiate a stage object
Stage *MetricsStage::make_stage(const std::string &tag) {
  MetricsStage *stage = new MetricsStage(tag.c_str());
  if (stage == NULL) {
    LOG_ERROR("new MetricsStage failed");
    return NULL;
  }
  stage->set_properties();
  return stage;
}

// Set properties for this object set in stage specific properties
bool MetricsStage::set_properties() {
  std::string stage_name_str(stage_name_);
  std::map<std::string, std::string> section =
      get_properties()->get(stage_name_str);

  metric_report_interval_ = DateTime::SECONDS_PER_MIN;

  std::string key = METRCS_REPORT_INTERVAL;
  std::map<std::string, std::string>::iterator it = section.find(key);
  if (it != section.end()) {
    str_to_val(it->second, metric_report_interval_);
  }

  return true;
}

// Initialize stage params and validate outputs
bool MetricsStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  timer_stage_ = *(stgp++);

  MetricsReportEvent *report_event = new MetricsReportEvent();

  add_event(report_event);
  LOG_TRACE("Exit");
  return true;
}

// Cleanup after disconnection
void MetricsStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void MetricsStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  CompletionCallback *cb = new CompletionCallback(this, NULL);
  if (cb == NULL) {
    LOG_ERROR("Failed to new callback");

    event->done();

    return;
  }

  TimerRegisterEvent *tm_event =
      new TimerRegisterEvent(event, metric_report_interval_ * USEC_PER_SEC);
  if (tm_event == NULL) {
    LOG_ERROR("Failed to new TimerRegisterEvent");

    delete cb;

    event->done();

    return;
  }

  event->push_callback(cb);
  timer_stage_->add_event(tm_event);

  LOG_TRACE("Exit\n");
  return;
}

void MetricsStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  MetricsRegistry &metrics_registry = get_metrics_registry();

  metrics_registry.snapshot();
  metrics_registry.report();

  // do it again.
  add_event(event);

  LOG_TRACE("Exit\n");
  return;
}
