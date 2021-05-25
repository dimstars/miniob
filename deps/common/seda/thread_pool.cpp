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

#include "common/lang/mutex.h"
#include "common/log/log.h"

#include "common/seda/stage.h"
#include "common/seda/thread_pool.h"
namespace common {

extern bool &theEventHistoryFlag();

//! Constructor
/**
 * @param[in] threads The number of threads to create.
 *
 * @post thread pool has <i>threads</i> threads running
 */
Threadpool::Threadpool(unsigned int threads, const std::string &name)
  : runQueue(), eventhist(theEventHistoryFlag()), nthreads(0),
    threadsToKill(0), nIdles(0), killer("KillThreads"), name(name) {
  LOG_TRACE("Enter, thread number:%d", threads);
  MUTEX_INIT(&runMutex, NULL);
  COND_INIT(&runCond, NULL);
  MUTEX_INIT(&threadMutex, NULL);
  COND_INIT(&threadCond, NULL);
  addThreads(threads);
  LOG_TRACE("exit");
}

//! Destructor
/**
 * Kills all threads and destroys pool.
 *
 * @post all threads are destroyed and pool is destroyed
 */
Threadpool::~Threadpool() {
  LOG_TRACE("%s", "enter");
  // kill all the remaining service threads
  killThreads(nthreads);

  runQueue.clear();
  MUTEX_DESTROY(&runMutex);
  COND_DESTROY(&runCond);
  MUTEX_DESTROY(&threadMutex);
  COND_DESTROY(&threadCond);
  LOG_TRACE("%s", "exit");
}

//! Query number of threads.
/**
 * @return number of threads in the thread pool.
 */
unsigned int Threadpool::numThreads() {
  MUTEX_LOCK(&threadMutex);
  unsigned int result = nthreads;
  MUTEX_UNLOCK(&threadMutex);
  return result;
}

//! Add threads to the pool
/**
 * @param[in] threads Number of threads to add to the pool.
 *
 * @post  0 <= (# of threads in pool) - (original # of threads in pool)
 *        <= threads
 * @return number of thread successfully created
 */
unsigned int Threadpool::addThreads(unsigned int threads) {
  unsigned int i;
  pthread_t pThread;
  pthread_attr_t pThreadAttrs;
  LOG_TRACE("%s adding threads enter%d", name.c_str(), threads);
  // create all threads as detached.  We will not try to join them.
  pthread_attr_init(&pThreadAttrs);
  pthread_attr_setdetachstate(&pThreadAttrs, PTHREAD_CREATE_DETACHED);

  MUTEX_LOCK(&threadMutex);

  // attempt to start the requested number of threads
  for (i = 0; i < threads; i++) {
    int stat = pthread_create(&pThread, &pThreadAttrs, Threadpool::runThread,
                              (void *) this);
    if (stat != 0) {
      LOG_WARN("Failed to create one thread\n");
      break;
    }
  }
  nthreads += i;
  MUTEX_UNLOCK(&threadMutex);
  LOG_TRACE("%s%d", "adding threads exit", threads);
  return i;
}

//! Kill threads in pool
/**
 * Blocks until the requested number of threads are killed.  Won't
 * kill more than current number of threads.
 *
 * @param[in] threads Number of threads to kill.
 *
 * @post (original # of threads in pool) - (# of threads in pool)
 *       <= threads
 * @return number of threads successfully killed.
 */
unsigned int Threadpool::killThreads(unsigned int threads) {
  LOG_TRACE("%s%d", "enter - threads to kill", threads);
  MUTEX_LOCK(&threadMutex);

  // allow only one thread kill transaction at a time
  if (threadsToKill > 0) {
    MUTEX_UNLOCK(&threadMutex);
    return 0;
  }

  // check the limit
  if (threads > nthreads) {
    threads = nthreads;
  }

  // connect the kill thread stage to this pool
  killer.setPool(this);
  killer.connect();

  // generate an appropriate number of kill thread events...
  int i = genKillThreadEvents(threads);

  // set the counter and wait for events to be picked up.
  threadsToKill = i;
  COND_WAIT(&threadCond, &threadMutex);

  killer.disconnect();

  MUTEX_UNLOCK(&threadMutex);
  LOG_TRACE("%s", "exit");
  return i;
}

//! Internal thread kill.
/**
 * Internal operation called only when a thread kill event is processed.
 * Reduces the count of active threads, and, if this is the last pending
 * kill, signals the waiting killThreads method.
 */
void Threadpool::threadKill() {
  MUTEX_LOCK(&threadMutex);

  nthreads--;
  threadsToKill--;
  if (threadsToKill == 0) {
    // signal the condition, in case someone is waiting there...
    COND_SIGNAL(&threadCond);
  }

  MUTEX_UNLOCK(&threadMutex);
}

//! Internal generate kill thread events
/**
 * Internal operation called by killThreads(). Generates the requested
 * number of kill thread events and schedules them.
 *
 * @pre  thread mutex is locked.
 * @pre  toKill <= current number of threads
 * @return number of kill thread events successfully scheduled
 */
unsigned int Threadpool::genKillThreadEvents(unsigned int toKill) {
  LOG_TRACE("%s%d", "enter", toKill);
  assert(MUTEX_TRYLOCK(&threadMutex) != 0);
  assert(toKill <= nthreads);

  unsigned int i;
  for (i = 0; i < toKill; i++) {

    // allocate kill thread event and put it on the list...
    StageEvent *sevent = new StageEvent();
    if (sevent == NULL) {
      break;
    }
    killer.addEvent(sevent);
  }
  LOG_TRACE("%s%d", "exit", toKill);
  return i;
}

//! Schedule stage with some work
/**
 * Schedule a stage with some work to be done on the run queue.
 *
 * @param[in] stageP Reference to stage to be scheduled.
 *
 * @pre  stageP must have a non-empty queue.
 * @post stageP is scheduled on the run queue.
 */
void Threadpool::schedule(Stage *stageP) {
  assert(!stageP->qempty());

  MUTEX_LOCK(&runMutex);
  bool wasEmpty = runQueue.empty();
  runQueue.push_back(stageP);
  // let current thread continue to run the target stage if there is
  // only one event and the target stage is in the same thread pool
  if (wasEmpty == false || this != getThreadPoolPtr()) {
    // wake up if there is idle thread
    if (nIdles > 0) {
      COND_SIGNAL(&runCond);
    }
  }
  MUTEX_UNLOCK(&runMutex);
}

//! Get name of thread pool
const std::string &Threadpool::getName() { return name; }

//! Internal thread control function
/**
 * Function which contains the control loop for each service thread.
 * Should not be called except when a thread is created.
 */
void *Threadpool::runThread(void *poolPtr) {
  Threadpool *poolP = (Threadpool *) poolPtr;

  // save thread pool pointer
  setThreadPoolPtr(poolP);

  // this is not portable, but is easier to map to LWP
  s64_t threadid = gettid();
  LOG_INFO("threadid = %llx, threadname = %s\n", threadid,
           poolP->getName().c_str());

  // enter a loop where we continuously look for events from Stages on
  // the runQueue and handle the event.
  while (1) {
    MUTEX_LOCK(&(poolP->runMutex));

    // wait for some stage to be scheduled
    while (poolP->runQueue.empty()) {
      (poolP->nIdles)++;
      COND_WAIT(&(poolP->runCond), &(poolP->runMutex));
      (poolP->nIdles)--;
    }

    assert(!poolP->runQueue.empty());
    Stage *runStage = *(poolP->runQueue.begin());
    poolP->runQueue.pop_front();
    MUTEX_UNLOCK(&(poolP->runMutex));

    StageEvent *event = runStage->removeEvent();

    // need to check if this is a rescheduled callback
    if (event->isCallback()) {
#ifdef ENABLE_STAGE_LEVEL_TIMEOUT
      // check if the event has timed out.
      if (event->hasTimedOut()) {
        event->doneTimeout();
      } else {
        event->doneImmediate();
      }
#else
      event->doneImmediate();
#endif
    } else {
      if (poolP->eventhist) {
        event->saveStage(runStage, StageEvent::HANDLE_EV);
      }

#ifdef ENABLE_STAGE_LEVEL_TIMEOUT
      // check if the event has timed out
      if (event->hasTimedOut()) {
        event->done();
      } else {
        runStage->handleEvent(event);
      }
#else
      runStage->handleEvent(event);
#endif
    }
    runStage->releaseEvent();
  }
  LOG_TRACE("exit %p", poolPtr);
  LOG_INFO("Begin to exit, threadid = %llx, threadname = %s", threadid,
           poolP->getName().c_str());

  // the dummy compiler need this
  pthread_exit(NULL);
}

pthread_key_t Threadpool::poolPtrKey;

void Threadpool::createPoolKey() {
  // init the thread specific to store thread pool pointer
  // this is called in main thread, so no pthread_once is needed
  pthread_key_create(&poolPtrKey, NULL);
}

void Threadpool::delPoolKey() { pthread_key_delete(poolPtrKey); }

void Threadpool::setThreadPoolPtr(const Threadpool *thdPool) {
  pthread_setspecific(poolPtrKey, thdPool);
}

const Threadpool *Threadpool::getThreadPoolPtr() {
  return (const Threadpool *) pthread_getspecific(poolPtrKey);
}

} //namespace common