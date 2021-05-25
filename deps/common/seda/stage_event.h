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
#ifndef __COMMON_SEDA_STAGE_EVENT_H__
#define __COMMON_SEDA_STAGE_EVENT_H__

// Include Files
#include <list>
#include <map>
#include <string>
#include <time.h>

#include "common/defs.h"
namespace common {

class CompletionCallback;
class UserData;
class Stage;
class TimeoutInfo;

//! An event in a staged event-driven architecture
/**
 * Abstract base class for all events.  Each event can reference
 * a stack of completion callbacks. A callback is added to an event using
 * the pushCallback() method.  The first completion callback on the stack
 * is invoked by calling the done() method of StageEvent.  The Stage code
 * handling an event (either in handleEvent() or callbackEvent()) has
 * certain responsibilities regarding how the event and its associated
 * callbacks are processed. When a stage finishes processing an
 * event it has the following options:
 * <ul>
 * <li>Pass the event along to another stage for processing.  In this
 * case the responsibility to eventually call done() passes to the next
 * stage.
 * <li>Pass the event along to another stage for processing but add a
 * callback to the event for the current stage.  In this case, the stage
 * must create a callback object and use the pushCallback() interface
 * to add the callback to the top of the event's callback stack. Again,
 * the responsibility for calling done() passes to the next stage.
 * <li>Dispose of the event.  This is achieved by calling done().  After
 * calling done() the stage must not access the event again.  Note that
 * done() will result in callbacks attached to the event being executed
 * asynchronously by the threadpool of the stage which set the callback.
 * Calling doneImmediate() has the same effect as done(), except that the
 * callbacks are executed on the current stack.
 * </ul>
 */

class StageEvent {

 public:
  //! Constructor
  /**
   *  Should not create StageEvents on the stack.  done() assumes that
   *  event is dynamically allocated.
   */
  StageEvent();

  //! Destructor
  /**
   *  Should only be called from done(), or from Stage class
   *  during cleanup.  Public for now, because constructor is public.
   */
  virtual ~StageEvent();

  //! Processing for this event is done; execute callbacks
  // this will trigger thread switch if there are callbacks,  this will be async
  // Calling doneImmediate won't trigger thread switch, this will be synchonized
  void done();

  //! Processing for this event is done; execute callbacks immediately
  // Calling doneImmediate won't trigger thread switch, this will be synchonized
  void doneImmediate();

  //! Processing for this event is done if the event has timed out
  /**
   *  \c timeoutEvent() will be called instead of \c callbackEvent()
   *  if the event has timed out.
   */
  void doneTimeout();

  //! Set the completion callback
  void pushCallback(CompletionCallback *cb);

  //! Set the originating event that caused this event.
  /**
   *  The caller is responsible for recovering the memory associated with
   *  the \c UserData object.
   */
  void setUserData(UserData *u);

  //! Get the originating event the caused this event.
  UserData *getUserData();

  //! True if event represents a callback
  bool isCallback() { return cbFlag; }

 private:
  CompletionCallback *compCB; //!< completion callback stack for this event
  UserData *ud;               //!< user data associated with event by caller

 private:
  // Interface to allow callbacks to be run on target stage's threads
  void markCallback() { cbFlag = true; }
  void clearCallback() { cbFlag = false; }

  bool cbFlag; //!< true if this event is a callback

 public:
  // Interface for collecting debugging information

  typedef enum { HANDLE_EV = 0, CALLBACK_EV, TIMEOUT_EV } HistType;

  //! Add stage to list of stages which have handled this event
  void saveStage(Stage *stg, HistType type);

 private:
  typedef std::pair<Stage *, HistType> HistEntry;

  std::list<HistEntry> *history; //!< List of stages which have handled ev
  u32_t stageHops;               //!< Number of stages which have handled ev

 public:
  //! Set a timeout info into the event
  /**
   * @param[in] deadline  deadline of the timeout
   */
  void setTimeoutInfo(time_t deadline);

  //! Share a timeout info with another \c StageEvent
  void setTimeoutInfo(const StageEvent &ev);

  //! If the event has timed out (and should be dropped)
  bool hasTimedOut();

 private:
  //! Set a timeout info into the event
  void setTimeoutInfo(TimeoutInfo *tmi);

  TimeoutInfo *tmInfo; //!< the timeout info for this event
};

/**
 *
 *  \brief An opaque data structure that can be associated with any event to
 *  maintain the state of the calling stage.
 *
 *  Setting the \c UserData member is optional.  The caller may elect to set
 *  the \c UserData member to maintain state across a request to another
 *  stage.  If the \c UserData member is used, the calling state should create
 *  a new class derived from \c UserData to store state relevant to its
 *  processing.  When the called stage invokes the \c CompletionCallback, the
 *  originating stage can access the \c UserData member to recover its state.
 */
class UserData {
 public:
  /**
   *  \brief A virtual destructor to enable the use of dynamic casts.
   */
  virtual ~UserData() { return; }
};

bool &theEventHistoryFlag();
u32_t &theMaxEventHops();

} //namespace common
#endif // __COMMON_SEDA_STAGE_EVENT_H__
