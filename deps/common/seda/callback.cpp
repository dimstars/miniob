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
// Created by Longda on 2010
//

// Include Files
#include "common/seda/callback.h"

#include <assert.h>

#include "common/log/log.h"
#include "common/seda/stage.h"
#include "common/seda/stage_event.h"
namespace common {

extern bool &theEventHistoryFlag();

/**
 * @author Longda
 * @date   3/27/07
 *
 * Implementation of CompletionCallback class.
 */

//! Constructor
CompletionCallback::CompletionCallback(Stage *trgt, CallbackContext *ctx)
  : targetStage(trgt), context(ctx), nextCb(NULL),
    evHistFlag(theEventHistoryFlag()) {}

//! Destructor
CompletionCallback::~CompletionCallback() {
  if (context) {
    delete context;
  }
  if (nextCb) {
    delete nextCb;
  }
}

//! Push onto a callback stack
void CompletionCallback::pushCallback(CompletionCallback *next) {
  ASSERT((!nextCb), "%s", "cannot push a callback twice");

  nextCb = next;
}

//! Pop off of a callback stack
CompletionCallback *CompletionCallback::popCallback() {
  CompletionCallback *retVal = nextCb;

  nextCb = NULL;
  return retVal;
}

//! One event is complete
void CompletionCallback::eventDone(StageEvent *ev) {

  if (evHistFlag) {
    ev->saveStage(targetStage, StageEvent::CALLBACK_EV);
  }
  targetStage->callback_event(ev, context);
}

//! Reschedule callback on target stage thread
void CompletionCallback::eventReschedule(StageEvent *ev) {
  targetStage->add_event(ev);
}

void CompletionCallback::eventTimeout(StageEvent *ev) {
  LOG_DEBUG("to call eventTimeout for stage %s", targetStage->get_name());
  if (evHistFlag) {
    ev->saveStage(targetStage, StageEvent::TIMEOUT_EV);
  }
  targetStage->timeout_event(ev, context);
}

} //namespace common