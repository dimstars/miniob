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
#include <assert.h>
#include <stdlib.h>

#include "common/defs.h"
#include "common/lang/mutex.h"
#include "common/log/log.h"

#include "common/seda/callback.h"
#include "common/seda/stage_event.h"
#include "common/time/timeout_info.h"
namespace common {

//! Constructor
StageEvent::StageEvent()
  : compCB(NULL), ud(NULL), cbFlag(false), history(NULL), stageHops(0),
    tmInfo(NULL) {}

//! Destructor
StageEvent::~StageEvent() {
  // clear all pending callbacks
  while (compCB) {
    CompletionCallback *top = compCB;
    compCB = compCB->popCallback();
    delete top;
  }

  // delete the history if present
  if (history) {
    history->clear();
    delete history;
  }

  if (tmInfo) {
    tmInfo->detach();
    tmInfo = NULL;
  }
}

//! Processing for this event is done; callbacks executed
void StageEvent::done() {
  CompletionCallback *top;

  if (compCB) {
    top = compCB;
    markCallback();
    top->eventReschedule(this);
  } else {
    delete this;
  }
}

//! Processing for this event is done; callbacks executed immediately
void StageEvent::doneImmediate() {
  CompletionCallback *top;

  if (compCB) {
    top = compCB;
    clearCallback();
    compCB = compCB->popCallback();
    top->eventDone(this);
    delete top;
  } else {
    delete this;
  }
}

void StageEvent::doneTimeout() {
  CompletionCallback *top;

  if (compCB) {
    top = compCB;
    clearCallback();
    compCB = compCB->popCallback();
    top->eventTimeout(this);
    delete top;
  } else {
    delete this;
  }
}

//! Push the completion callback onto the stack
void StageEvent::pushCallback(CompletionCallback *cb) {
  cb->pushCallback(compCB);
  compCB = cb;
}

void StageEvent::setUserData(UserData *u) {
  ud = u;
  return;
}

UserData *StageEvent::getUserData() { return ud; }

//! Add stage to list of stages which have handled this event
void StageEvent::saveStage(Stage *stg, HistType type) {
  if (!history) {
    history = new std::list<HistEntry>;
  }
  if (history) {
    history->push_back(std::make_pair(stg, type));
    stageHops++;
    ASSERT(stageHops <= theMaxEventHops(), "Event exceeded max hops");
  }
}

void StageEvent::setTimeoutInfo(TimeoutInfo *tmi) {
  // release the previous timeout info
  if (tmInfo) {
    tmInfo->detach();
  }

  tmInfo = tmi;
  if (tmInfo) {
    tmInfo->attach();
  }
}

void StageEvent::setTimeoutInfo(time_t deadline) {
  TimeoutInfo *tmi = new TimeoutInfo(deadline);
  setTimeoutInfo(tmi);
}

void StageEvent::setTimeoutInfo(const StageEvent &ev) {
  setTimeoutInfo(ev.tmInfo);
}

bool StageEvent::hasTimedOut() {
  if (!tmInfo) {
    return false;
  }

  return tmInfo->hasTimedOut();
}

//! Accessor function which wraps value for max hops an event is allowed
u32_t &theMaxEventHops() {
  static u32_t maxEventHops = 0;
  return maxEventHops;
}

//! Accessor function which wraps value for event history flag
bool &theEventHistoryFlag() {
  static bool eventHistoryFlag = false;
  return eventHistoryFlag;
}

} //namespace common