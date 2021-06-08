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
#include "common/seda/kill_thread.h"

#include <assert.h>

#include "common/seda/thread_pool.h"
namespace common {


//! Notify the pool and kill the thread
/**
 * @param[in] event Pointer to event that must be handled.
 *
 * @post  Call never returns.  Thread is killed.  Pool is notified.
 */
void KillThreadStage::handle_event(StageEvent *event) {
  get_pool()->thread_kill();
  event->done();
  this->release_event();
  pthread_exit(0);
}

//! Process properties of the classes
/**
 * @pre class members are uninitialized
 * @post initializing the class members
 * @return the class object
 */
Stage *KillThreadStage::make_stage(const std::string &tag) {
  return new KillThreadStage(tag.c_str());
}

bool KillThreadStage::set_properties() {
  // nothing to do
  return true;
}

} //namespace common