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
#include "common/seda/event_dispatcher.h"
namespace common {

//! Constructor
EventDispatcher::EventDispatcher(const char *tag)
  : Stage(tag), eventStore(), nextStage(NULL) {
  LOG_TRACE("enter\n");

  pthread_mutexattr_t attr;

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  pthread_mutex_init(&eventLock, &attr);

  LOG_TRACE("exit\n");
}

//! Destructor
EventDispatcher::~EventDispatcher() {
  LOG_TRACE("enter\n");
  pthread_mutex_destroy(&eventLock);
  LOG_TRACE("exit\n");
}

//! Process an event
/**
 * Check if the event can be dispatched. If not, hash it and store
 * it.  If so, send it on to the next stage.
 */
void EventDispatcher::handle_event(StageEvent *event) {
  LOG_TRACE("enter\n");

  std::string hash;
  DispatchContext *ctx = NULL;
  status_t stat;

  pthread_mutex_lock(&eventLock);
  stat = dispatchEvent(event, ctx, hash);
  if (stat == SEND_EVENT) {
    nextStage->add_event(event);
  } else if (stat == STORE_EVENT) {
    StoredEvent se(event, ctx);

    eventStore[hash].push_back(se);
  } else {
    LOG_ERROR("Dispatch event failure\n");
    // in this case, dispatchEvent is assumed to have disposed of event
  }
  pthread_mutex_unlock(&eventLock);

  LOG_TRACE("exit\n");
}

//! Initialize stage params and validate outputs
bool EventDispatcher::initialize() {
  bool retVal = true;

  if (next_stage_list_.size() != 1) {
    retVal = false;
  } else {
    nextStage = *(next_stage_list_.begin());
  }
  return retVal;
}

//! Cleanup stage after disconnection
/**
 * Call done() on any events left over in the eventStore.
 */
void EventDispatcher::cleanup() {
  pthread_mutex_lock(&eventLock);

  // for each hash chain...
  for (EventHash::iterator i = eventStore.begin(); i != eventStore.end(); i++) {

    // for each event on the chain
    for (std::list<StoredEvent>::iterator j = i->second.begin();
         j != i->second.end(); j++) {
      j->first->done();
    }
    i->second.clear();
  }
  eventStore.clear();

  pthread_mutex_unlock(&eventLock);
}

//! Wake up a stored event
bool EventDispatcher::wakeupEvent(std::string hashkey) {
  bool sent = false;
  EventHash::iterator i;

  i = eventStore.find(hashkey);
  if (i != eventStore.end()) {

    // find the event and remove it from the current queue
    StoredEvent targetEv = *(i->second.begin());
    i->second.pop_front();
    if (i->second.size() == 0) {
      eventStore.erase(i);
    }

    // try to dispatch the event again
    status_t stat = dispatchEvent(targetEv.first, targetEv.second, hashkey);
    if (stat == SEND_EVENT) {
      nextStage->add_event(targetEv.first);
      sent = true;
    } else if (stat == STORE_EVENT) {
      eventStore[hashkey].push_back(targetEv);
    } else {
      LOG_ERROR("Dispatch event failure\n");
      // in this case, dispatchEvent is assumed to have disposed of event
    }
  }

  return sent;
}

} //namespace common