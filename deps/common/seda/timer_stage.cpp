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

#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>
#include <time.h>
#include <typeinfo>

#include "common/lang/mutex.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
namespace common {

#define TIMEVAL_EQUAL(t1, t2)                                                  \
  ((t1.tv_sec == t2.tv_sec) && (t1.tv_usec == t2.tv_usec))
#define TIMEVAL_LESS_THAN(t1, t2)                                              \
  ((t1.tv_sec < t2.tv_sec) ||                                                  \
   ((t1.tv_sec == t2.tv_sec) && (t1.tv_usec < t2.tv_usec)))

struct timeval subTimeval(const struct timeval *t1, const struct timeval *t2) {
  struct timeval result;
  result.tv_sec = t1->tv_sec - t2->tv_sec;
  result.tv_usec = t1->tv_usec - t2->tv_usec;
  while (result.tv_usec < 0) {
    --result.tv_sec;
    result.tv_usec += USEC_PER_SEC;
  }
  return result;
}

struct timeval addTimeval(const struct timeval *t1, const struct timeval *t2) {
  struct timeval result;
  result.tv_sec = t1->tv_sec + t2->tv_sec;
  result.tv_usec = t1->tv_usec + t2->tv_usec;
  while (result.tv_usec >= USEC_PER_SEC) {
    ++result.tv_sec;
    result.tv_usec -= USEC_PER_SEC;
  }
  return result;
}

void realtimeToMonotonic(const struct timeval *time_RT,
                         struct timeval *time_Mono) {

  struct timeval time_now;
  gettimeofday(&time_now, NULL);
  struct timeval time_offset;
  time_offset = subTimeval(time_RT, &time_now);

  struct timespec time_ts;
  clock_gettime(CLOCK_MONOTONIC, &time_ts);

  struct timeval time_temp;
  time_temp.tv_sec = time_ts.tv_sec;
  time_temp.tv_usec = time_ts.tv_nsec / NSEC_PER_USEC;
  time_temp = addTimeval(&time_temp, &time_offset);

  time_Mono->tv_sec = time_temp.tv_sec;
  time_Mono->tv_usec = time_temp.tv_usec;
}

u64_t TimerToken::nextNonce() {
  static u64_t nonce_cntr = 0;
  static pthread_mutex_t tt_mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&tt_mutex);
  u64_t n = nonce_cntr++;
  pthread_mutex_unlock(&tt_mutex);

  return n;
}

TimerToken::TimerToken() {
  struct timeval t;
  memset(&t, 0, sizeof(struct timeval));
  u64_t n = nextNonce();
  set(t, n);
  return;
}

TimerToken::TimerToken(const struct timeval &t) {
  u64_t n = nextNonce();
  set(t, n);
  return;
}

TimerToken::TimerToken(const TimerToken &tt) {
  set(tt.time, tt.nonce);
  return;
}

void TimerToken::set(const struct timeval &t, u64_t n) {
  memcpy(&time, &t, sizeof(struct timeval));
  nonce = n;
  return;
}

const struct timeval &TimerToken::getTime() const { return time; }

u64_t TimerToken::getNonce() const { return nonce; }

bool TimerToken::operator<(const TimerToken &other) const {
  if (TIMEVAL_LESS_THAN(time, other.time))
    return true;
  if (TIMEVAL_EQUAL(time, other.time))
    return (nonce < other.nonce);
  return false;
}

TimerToken &TimerToken::operator=(const TimerToken &src) {
  set(src.time, src.nonce);
  return *this;
}

std::string TimerToken::toString() const {
  std::string s;
  std::ostringstream ss(s);
  ss << time.tv_sec << ":" << time.tv_usec << "-" << nonce;
  return ss.str();
}

TimerRegisterEvent::TimerRegisterEvent(StageEvent *cb, u64_t time_relative_usec)
  : TimerEvent(), timer_cb(cb), token() {
  struct timespec timer_spec;
  clock_gettime(CLOCK_MONOTONIC, &timer_spec);

  timer_when.tv_sec = timer_spec.tv_sec;
  timer_when.tv_usec = (timer_spec.tv_nsec / NSEC_PER_USEC);

  timer_when.tv_usec += time_relative_usec;
  if (timer_when.tv_usec >= USEC_PER_SEC) {
    timer_when.tv_sec += (timer_when.tv_usec / USEC_PER_SEC);
    timer_when.tv_usec = (timer_when.tv_usec % USEC_PER_SEC);
  }

  return;
}

TimerRegisterEvent::TimerRegisterEvent(StageEvent *cb,
                                       struct timeval &time_absolute)
  : TimerEvent(), timer_cb(cb), token() {
  realtimeToMonotonic(&time_absolute, &timer_when);
  return;
}

TimerRegisterEvent::~TimerRegisterEvent() { return; }

const struct timeval &TimerRegisterEvent::getTime() { return timer_when; }

StageEvent *TimerRegisterEvent::getCallbackEvent() { return timer_cb; }

StageEvent *TimerRegisterEvent::adoptCallbackEvent() {
  StageEvent *e = timer_cb;
  timer_cb = NULL;
  return e;
}

void TimerRegisterEvent::setCancelToken(const TimerToken &t) {
  token = t;
  return;
}

std::unique_ptr<const TimerToken> TimerRegisterEvent::getCancelToken() {
  const TimerToken *token_cp = new TimerToken(token);
  std::unique_ptr<const TimerToken> token_ptr(token_cp);
  return token_ptr;
}

TimerCancelEvent::TimerCancelEvent(const TimerToken &cancel_token)
  : TimerEvent(), token(cancel_token), cancelled(false) {
  return;
}

TimerCancelEvent::~TimerCancelEvent() { return; }

const TimerToken &TimerCancelEvent::getToken() { return token; }

void TimerCancelEvent::setSuccess(bool s) {
  cancelled = s;
  return;
}

bool TimerCancelEvent::getSuccess() { return cancelled; }

TimerStage::TimerStage(const char *tag)
  : Stage(tag), timer_queue(&TimerStage::timerTokenLessThan), shutdown(false),
    num_events(0), timer_thread_id(0) {
  pthread_mutex_init(&timer_mutex, NULL);
  pthread_condattr_t condattr;
  pthread_condattr_init(&condattr);
#ifdef LINUX
  pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);
#endif
  pthread_cond_init(&timer_condv, &condattr);
  pthread_condattr_destroy(&condattr);
  return;
}

TimerStage::~TimerStage() {
  for (timer_queue_t::iterator i = timer_queue.begin(); i != timer_queue.end();
       ++i) {
    delete i->second;
  }

  num_events = 0;

  pthread_mutex_destroy(&timer_mutex);
  pthread_cond_destroy(&timer_condv);

  return;
}

Stage *TimerStage::makeStage(const std::string &tag) {
  TimerStage *s = new TimerStage(tag.c_str());
  ASSERT(s != NULL, "Failed to instantiate stage.");
  if (!s->setProperties()) {
    LOG_PANIC("failed to set properties.\n");
    delete s;
    s = NULL;
  }

  return s;
}

bool TimerStage::setProperties() {
  // No configuration is stored in the system properties.
  return true;
}

bool TimerStage::initialize() {
  // The TimerStage does not send messages to any other stage.
  ASSERT(nextStageList.size() == 0, "Invalid NextStages list.");

  // Start the thread to maintain the timer
  const pthread_attr_t *thread_attrs = NULL;
  void *thread_args = (void *) this;
  int status = pthread_create(&timer_thread_id, thread_attrs,
                              &TimerStage::startTimerThread, thread_args);
  if (status != 0)
    LOG_ERROR("failed to create timer thread: status=%d\n", status);

  return (status == 0);
}

u32_t TimerStage::getNumEvents() { return num_events; }

void TimerStage::disconnectPrepare() {
  LOG_INFO("received signal to initiate shutdown.\n");
  pthread_mutex_lock(&timer_mutex);
  shutdown = true;
  pthread_cond_signal(&timer_condv);
  pthread_mutex_unlock(&timer_mutex);

  LOG_TRACE("waiting for timer maintenance thread to terminate.\n");
  void **return_val_ptr = NULL;
  int status;
  status = pthread_join(timer_thread_id, return_val_ptr);
  LOG_TRACE("timer maintenance thread terminated: status=%d\n", status);

  return;
}

void TimerStage::handleEvent(StageEvent *event) {
  TimerEvent *e = dynamic_cast<TimerEvent *>(event);
  if (e == NULL) {
    LOG_WARN("received event of unexpected type: typeid=%s\n",
             typeid(*event).name());
    return; // !!! EARLY EXIT !!!
  }

  TimerRegisterEvent *register_ev = dynamic_cast<TimerRegisterEvent *>(event);
  if (register_ev != NULL) {
    registerTimer(*register_ev);
    return; // !!! EARLY EXIT !!!
  }

  TimerCancelEvent *cancel_ev = dynamic_cast<TimerCancelEvent *>(event);
  if (cancel_ev != NULL) {
    cancelTimer(*cancel_ev);
    return; // !!! EARLY EXIT !!!
  }

  return;
}

void TimerStage::callbackEvent(StageEvent *e, CallbackContext *ctx) { return; }

void TimerStage::registerTimer(TimerRegisterEvent &reg_ev) {
  const TimerToken tt(reg_ev.getTime());

  LOG_TRACE("registering event: token=%s\n", tt.toString().c_str());

  bool check_timer = false;
  pthread_mutex_lock(&timer_mutex);

  // add the event to the timer queue
  StageEvent *timer_cb = reg_ev.adoptCallbackEvent();
  std::pair<timer_queue_t::iterator, bool> result =
    timer_queue.insert(std::make_pair(tt, timer_cb));
  ASSERT(result.second,
         "Internal error--"
         "failed to register timer because token is not unique.");
  ++num_events;

  // if event was added to the head of queue, schedule a timer check
  if (result.first == timer_queue.begin())
    check_timer = true;

  pthread_mutex_unlock(&timer_mutex);

  reg_ev.setCancelToken(tt);
  reg_ev.done();

  if (check_timer)
    triggerTimerCheck();

  return;
}

void TimerStage::cancelTimer(TimerCancelEvent &cancel_ev) {
  pthread_mutex_lock(&timer_mutex);
  bool success = false;
  timer_queue_t::iterator it = timer_queue.find(cancel_ev.getToken());
  if (it != timer_queue.end()) {
    success = true;

    // delete the canceled timer event
    delete it->second;

    timer_queue.erase(it);
    --num_events;
  }
  pthread_mutex_unlock(&timer_mutex);

  LOG_DEBUG("cancelling event: token=%s, success=%d\n",
            cancel_ev.getToken().toString().c_str(), (int) success);

  cancel_ev.setSuccess(success);
  cancel_ev.done();

  return;
}

void TimerStage::triggerTimerCheck() {
  LOG_TRACE("signaling timer thread to complete timer check\n");

  pthread_mutex_lock(&timer_mutex);
  pthread_cond_signal(&timer_condv);
  pthread_mutex_unlock(&timer_mutex);

  return;
}

void *TimerStage::startTimerThread(void *arg) {
  TimerStage *tstage = static_cast<TimerStage *>(arg);
  ASSERT(tstage != NULL, "Internal error--failed to start timer thread.");
  tstage->checkTimer();
  return NULL;
}

void TimerStage::checkTimer() {
  pthread_mutex_lock(&timer_mutex);

  while (true) {
    struct timespec ts_now;
    clock_gettime(CLOCK_MONOTONIC, &ts_now);

    struct timeval now;
    now.tv_sec = ts_now.tv_sec;
    now.tv_usec = ts_now.tv_nsec / NSEC_PER_USEC;

    LOG_TRACE("checking timer: sec=%ld, usec=%ld\n", now.tv_sec, now.tv_usec);

    // Trigger all events for which the trigger time has already passed.
    timer_queue_t::iterator first = timer_queue.begin();
    timer_queue_t::iterator last;
    std::list < StageEvent * > done_events;
    for (last = first; last != timer_queue.end(); ++last) {
      if (TIMEVAL_LESS_THAN(now, last->first.getTime()))
        break;
      done_events.push_back(last->second);
    }
    timer_queue.erase(first, last);
    if (!done_events.empty()) {
      // It is ok to hold the mutex while executing this loop.
      // Triggering the events only enqueues the event on the
      // caller's queue--it does not perform any real work.
      for (std::list<StageEvent *>::iterator i = done_events.begin();
           i != done_events.end(); ++i) {
        LOG_TRACE("triggering timer event: sec=%ld, usec=%ld, typeid=%s\n",
                  now.tv_sec, now.tv_usec, typeid(**i).name());
        (*i)->done();
        --num_events;
      }
    }
    done_events.clear();

    // Check if the 'shutdown' signal has been received.  The
    // stage must not release the mutex between this check and the
    // call to wait on the condition variable.
    if (shutdown) {
      LOG_INFO("received shutdown signal, abandoning timer maintenance\n");
      break; // !!! EARLY EXIT !!!
    }

    // Sleep until the next service interval.
    first = timer_queue.begin();
    if (first == timer_queue.end()) {
      // If no timer events are registered, sleep indefinately.
      // (When new events are registered, the condition variable
      // will be signalled to allow service to resume.)
      LOG_TRACE("sleeping indefinately\n");
      pthread_cond_wait(&timer_condv, &timer_mutex);
    } else {
      // If timer events are registered, sleep until the first
      // event should be triggered.
      struct timespec ts;
      ts.tv_sec = first->first.getTime().tv_sec;
      ts.tv_nsec = first->first.getTime().tv_usec * NSEC_PER_USEC;

      LOG_TRACE("sleeping until next deadline: sec=%ld, nsec=%ld\n", ts.tv_sec,
                ts.tv_nsec);
      pthread_cond_timedwait(&timer_condv, &timer_mutex, &ts);
    }
  }

  pthread_mutex_unlock(&timer_mutex);

  return;
}

bool TimerStage::timerTokenLessThan(const TimerToken &tt1,
                                    const TimerToken &tt2) {
  return (tt1 < tt2);
}

} //namespace common