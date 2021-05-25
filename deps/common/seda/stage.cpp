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
#include <string.h>

#include "common/defs.h"
#include "common/lang/mutex.h"
#include "common/lang/string.h"
#include "common/log/log.h"

#include "common/seda/init.h"
#include "common/seda/stage.h"
#include "common/seda/thread_pool.h"
namespace common {


//! Constructor
/**
 * @param[in] tag     The label that identifies this stage.
 *
 * @pre  tag is non-null and points to null-terminated string
 * @post event queue is empty
 * @post stage is not connected
 */
Stage::Stage(const char *tag)
    : eventList(), connected(false), eventRef(0), nextStageList() {
  LOG_TRACE("%s", "enter");
  assert(tag != NULL);

  MUTEX_INIT(&listMutex, NULL);
  COND_INIT(&disconnectCond, NULL);
  stageName = new char[strlen(tag) + 1];
  strcpy(stageName, tag);
  LOG_TRACE("%s", "exit");
}

//! Destructor
/**
 * @pre  stage is not connected
 * @post pending events are deleted and stage is destroyed
 */
Stage::~Stage() {
  LOG_TRACE("%s", "enter");
  assert(!connected);
  MUTEX_LOCK(&listMutex);
  while (eventList.size() > 0) {
    delete *(eventList.begin());
    eventList.pop_front();
  }
  MUTEX_UNLOCK(&listMutex);
  nextStageList.clear();

  MUTEX_DESTROY(&listMutex);
  COND_DESTROY(&disconnectCond);
  delete[] stageName;
  LOG_TRACE("%s", "exit");
}

//! Connect this stage to pipeline and threadpool
/**
 * Connect the output of this stage to the inputs of the stages in
 * the provided stage list.  Each subclass will validate the provided
 * stage list to be sure it is appropriate.  If the validation succeeds,
 * connect to the Threadpool and start processing.
 *
 * @param[in,out] stageList Stages that come next in the pipeline.
 * @param[in]     pool      Threadpool which will handle events
 *
 * @pre  stage is not connected
 * @post nextStageList == original stageList
 * @post stageList is empty
 * @post thPool == pool
 * @return true if the connection succeeded, else false
 */
bool Stage::connect() {
  LOG_TRACE("%s%s", "enter", stageName);
  assert(!connected);
  assert(thPool != NULL);

  bool success = false;
  unsigned int backlog = 0;

  success = initialize();
  if (success) {
    MUTEX_LOCK(&listMutex);
    backlog = eventList.size();
    eventRef = backlog;
    connected = true;
    MUTEX_UNLOCK(&listMutex);
  }

  // if connection succeeded, schedule all the events in the queue
  if (connected) {
    while (backlog > 0) {
      thPool->schedule(this);
      backlog--;
    }
  }

  LOG_TRACE("%s%s%d", "exit", stageName, connected);
  return success;
}

//! Disconnect this stage from the pipeline and threadpool
/**
 * Block stage from being scheduled.  Wait for currently processing
 * and scheduled events to complete, then disconnect from the threadpool.
 * Disconnect the output of this stage from the inputs of the stages in the
 * nextStageList.
 *
 * @pre    stage is connected
 * @post   nextStageList empty
 * @post   thPool NULL
 * @post   stage is not connected
 */
void Stage::disconnect() {
  assert(connected == true);

  LOG_TRACE("%s%s", "enter", stageName);
  MUTEX_LOCK(&listMutex);
  disconnectPrepare();
  connected = false;
  while (eventRef > 0) {
    COND_WAIT(&disconnectCond, &listMutex);
  }
  thPool = NULL;
  nextStageList.clear();
  cleanup();
  MUTEX_UNLOCK(&listMutex);
  LOG_TRACE("%s%s", "exit", stageName);
}

//! Add an event to the queue.
/**
 * @param[in] event Event to add to queue.
 *
 * @pre  event non-null
 * @post event added to the end of event queue
 * @post event must not be de-referenced by caller after return
 */
void Stage::addEvent(StageEvent *event) {
  assert(event != NULL);

  MUTEX_LOCK(&listMutex);

  // add event to back of queue
  eventList.push_back(event);

  if (connected) {
    assert(thPool != NULL);

    eventRef++;
    MUTEX_UNLOCK(&listMutex);
    thPool->schedule(this);
  } else {
    MUTEX_UNLOCK(&listMutex);
  }
}

//! Query length of queue
/**
 * @return length of event queue.
 */
unsigned long Stage::qlen() const {
  unsigned long res;

  MUTEX_LOCK(&listMutex);
  res = eventList.size();
  MUTEX_UNLOCK(&listMutex);
  return res;
}

//! Query whether the queue is empty
/**
 * @return \c true if the queue is empty; \c false otherwise
 */
bool Stage::qempty() const {
  bool empty = false;

  MUTEX_LOCK(&listMutex);
  empty = eventList.empty();
  MUTEX_UNLOCK(&listMutex);
  return empty;
}

//! Remove an event from the queue.
/**
 * Remove an event from the queue.  Called only by service thread.
 *
 * @pre queue not empty.
 * @return first event on queue.
 * @post  first event on queue is removed from queue.
 */
StageEvent *Stage::removeEvent() {
  MUTEX_LOCK(&listMutex);

  assert(!eventList.empty());

  StageEvent *se = *(eventList.begin());
  eventList.pop_front();
  MUTEX_UNLOCK(&listMutex);

  return se;
}

//! Release ref on stage from event.
/**
 * Release event reference on stage.  Called only by service thread.
 *
 * @post event ref count on stage is decremented
 */
void Stage::releaseEvent() {
  MUTEX_LOCK(&listMutex);
  eventRef--;
  if (!connected && eventRef == 0) {
    COND_SIGNAL(&disconnectCond);
  }
  MUTEX_UNLOCK(&listMutex);
}

} //namespace common