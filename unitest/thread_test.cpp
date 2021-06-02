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
// Created by Longda on 2021
//

#include <iostream>
#include <pthread.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "common/log/log.h"

#include "thread_test.h"

using namespace common;

ThreadTest::ThreadTest() {
  // TODO Auto-generated constructor stub
  memset(param, 0, sizeof(param));

  MUTEX_INIT(&mutex, NULL);
  COND_INIT(&cond, NULL);

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

  MUTEX_INIT(&dead_mutex1, &attr);
  MUTEX_INIT(&dead_mutex2, &attr);
}

ThreadTest::~ThreadTest() {
  // TODO Auto-generated destructor stub
  MUTEX_DESTROY(&mutex);
  COND_DESTROY(&cond);

  MUTEX_DESTROY(&dead_mutex1);
  MUTEX_DESTROY(&dead_mutex2);
}

void *log_loop(void *param) {
  int index = *(int *)param;
  int i = 0;
  while (i < 100) {
    i++;
    LOG_INFO("index:%d --> %d", index, i);
  }

  return NULL;
}

void *runFunc(void *param) { return log_loop(param); }

void *ThreadTest::testCond(void *param) {

  std::cout << "Thread has been started" << std::endl;

  ThreadTest *instance = (ThreadTest *)(param);

  sleep(1);

  MUTEX_LOCK(&instance->mutex);
  std::cout << "Begin to receive cond" << std::endl;
  COND_WAIT(&instance->cond, &instance->mutex);

  std::cout << "receive one " << std::endl;

  COND_WAIT(&instance->cond, &instance->mutex);

  std::cout << "receive two " << std::endl;

  MUTEX_UNLOCK(&instance->mutex);

  pthread_exit(0);
}

int ThreadTest::create() {
  pthread_t tid[10];
  memset(tid, 0, sizeof(tid));

  LoggerFactory::init_default("testthread.log");

  int threadNum = 0;
  for (; threadNum < 10; threadNum++) {
    param[threadNum] = threadNum;
    int rc = pthread_create(&tid[threadNum], NULL, *runFunc, &param[threadNum]);
    if (rc) {
      std::cout << "failed to create thread" << std::endl;
      break;
    }
  }
  for (int i = 0; i <= threadNum; i++) {
    pthread_join(tid[i], NULL);
  }

  return 0;
}

int ThreadTest::startTestCond() {

  pthread_t tid;
  int rc = pthread_create(&tid, NULL, testCond, this);
  if (rc) {
    std::cerr << "Failed to create thread" << std::endl;
    return rc;
  }

  sleep(10);

  MUTEX_LOCK(&mutex);
  std::cout << "Begin send one signal" << std::endl;
  COND_SIGNAL(&cond);
  MUTEX_UNLOCK(&mutex);

  MUTEX_LOCK(&mutex);
  std::cout << "Send two signal" << std::endl;
  COND_SIGNAL(&cond);
  MUTEX_UNLOCK(&mutex);

  std::cout << "unlock mutex, wait join" << std::endl;

  pthread_join(tid, NULL);

  std::cout << "thread exit already" << std::endl;

  return 0;
}

void *ThreadTest::testDeadLock(void *param) {
  ThreadTest *instance = (ThreadTest *)param;

  std::cout << "slave started" << std::endl;

  std::cout << "slave thread begin lock dead_lock2" << std::endl;
  MUTEX_LOCK(&instance->dead_mutex2);

  std::cout << "slave get dead_lock1, begin lock dead_lock1" << std::endl;

  sleep(2);

  MUTEX_LOCK(&instance->dead_mutex1);

  std::cout << "slave get dead_lock1, begin unlock" << std::endl;
  MUTEX_UNLOCK(&instance->dead_mutex1);
  MUTEX_UNLOCK(&instance->dead_mutex2);

  std::cout << "slave  unlock all lock" << std::endl;

  pthread_exit(0);
}

int ThreadTest::startTestDeadLock() {

  pthread_t tid;
  int rc = pthread_create(&tid, NULL, testDeadLock, this);
  if (rc) {
    std::cerr << "Failed to create thread" << std::endl;
    return rc;
  }

  std::cout << "master thread begin lock dead_lock1" << std::endl;
  MUTEX_LOCK(&dead_mutex1);

  std::cout << "master get dead_lock1, begin lock dead_lock2" << std::endl;

  sleep(2);

  MUTEX_LOCK(&dead_mutex2);

  std::cout << "master get dead_lock2, begin unlock" << std::endl;
  MUTEX_UNLOCK(&dead_mutex2);
  MUTEX_UNLOCK(&dead_mutex1);

  std::cout << "master  unlock all lock" << std::endl;

  pthread_join(tid, NULL);

  std::cout << "thread exit already" << std::endl;

  return 0;
}

int main(int argc, char **argv) {
  //    ThreadTest test;
  //
  //    //test.startTestCond();
  //    test.startTestDeadLock();
}
