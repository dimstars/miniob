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

#ifndef __COMMON_MM_MPOOL_H__
#define __COMMON_MM_MPOOL_H__

#include <queue>

#include "common/lang/mutex.h"
#include "common/log/log.h"
namespace common {

#define CLMPOOL_DEFAULT_ADDSIZE 16

template<class T>
class MemPool {
 public:
  MemPool() : mQueue(), mAddSize(CLMPOOL_DEFAULT_ADDSIZE) {
    MUTEX_INIT(&mLock, NULL);
  }

  ~MemPool() {
    MUTEX_LOCK(&mLock);
    while (mQueue.empty() == false) {
      T *item = mQueue.front();
      mQueue.pop();
      delete item;
    }

    MUTEX_UNLOCK(&mLock);
    MUTEX_DESTROY(&mLock);
  }

  int add(int addSize) {
    int ret = 0;

    MUTEX_LOCK(&mLock);
    for (int i = 0; i < addSize; i++) {
      T *item = new T();
      if (item == NULL) {
        ret = -1;
        break;
      }
      mQueue.push(item);
    }
    MUTEX_UNLOCK(&mLock);

    return ret;
  }

  int init(int initSize) { return add(initSize); }

  T *get() {
    T *ret = NULL;

    MUTEX_LOCK(&mLock);
    if (mQueue.empty() == true) {
      add(mAddSize);
    }

    if (mQueue.empty() == false) {
      ret = mQueue.front();
      mQueue.pop();
    }

    MUTEX_UNLOCK(&mLock);

    return ret;
  }

  void put(T *item) {
    MUTEX_LOCK(&mLock);

    mQueue.push(item);

    MUTEX_UNLOCK(&mLock);
  }

 private:
  std::queue<T *> mQueue;
  pthread_mutex_t mLock;
  int mAddSize;
};

} //namespace common
#endif /* __COMMON_MM_MPOOL_H__ */
