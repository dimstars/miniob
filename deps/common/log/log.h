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

#ifndef __COMMON_LOG_LOG_H__
#define __COMMON_LOG_LOG_H__

#include <assert.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <pthread.h>
#include <set>
#include <string.h>
#include <string>
#include <time.h>

#include "common/defs.h"

namespace common {

const int LOG_STATUS_OK = 0;
const int LOG_STATUS_ERR = 1;
const int LOG_MAX_LINE = 100000;

typedef enum {
  LOG_LEVEL_PANIC = 0,
  LOG_LEVEL_ERR = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_INFO = 3,
  LOG_LEVEL_DEBUG = 4,
  LOG_LEVEL_TRACE = 5,
  LOG_LEVEL_LAST
} LOG_LEVEL;

typedef enum {
  LOG_ROTATE_BYDAY = 0,
  LOG_ROTATE_BYSIZE,
  LOG_ROTATE_LAST
} LOG_ROTATE;

class Log {
 public:
  Log(const std::string &logName, const LOG_LEVEL logLevel = LOG_LEVEL_INFO,
      const LOG_LEVEL consoleLevel = LOG_LEVEL_WARN);
  ~Log(void);

  static int init(const std::string &logFile);

  /**
   * These functions won't output header information such as __FUNCTION__,
   * The user should control these information
   * If the header information should be outputed
   * please use LOG_PANIC, LOG_ERROR ...
   */
  template<class T>
  Log &operator<<(T message);

  template<class T>
  int Panic(T message);

  template<class T>
  int Error(T message);

  template<class T>
  int Warnning(T message);

  template<class T>
  int Info(T message);

  template<class T>
  int Debug(T message);

  template<class T>
  int Trace(T message);

  int Output(const LOG_LEVEL level, const char *module, const char *prefix,
             const char *f, ...);

  int SetConsoleLevel(const LOG_LEVEL consoleLevel);
  LOG_LEVEL GetConsoleLevel();

  int SetLogLevel(const LOG_LEVEL logLevel);
  LOG_LEVEL GetLogLevel();

  int SetRotateType(LOG_ROTATE rotateType);
  LOG_ROTATE GetRotateType();

  const char *PrefixMsg(const LOG_LEVEL level);

  /**
   * Set Default Module list
   * if one module is default module,
   * it will output whatever output level is lower than mLogLevel or not
   */
  void SetDefaultModule(const std::string &modules);
  bool CheckOutput(const LOG_LEVEL logLevel, const char *module);

  int Rotate(const int year = 0, const int month = 0, const int day = 0);

 private:
  void CheckParamValid();

  int RotateBySize();
  int RenameOldLogs();
  int RotateByDay(const int year, const int month, const int day);

  template<class T>
  int Out(const LOG_LEVEL consoleLevel, const LOG_LEVEL logLevel, T &message);

 private:
  pthread_mutex_t mLock;
  std::ofstream mOfs;
  std::string mLogName;
  LOG_LEVEL mLogLevel;
  LOG_LEVEL mConsoleLevel;

  typedef struct _LogDate {
    int mYear;
    int mMon;
    int mDay;
  } LogDate;
  LogDate mLogDate;
  int mLogLine;
  int mLogMaxLine;
  LOG_ROTATE mRotateType;

  typedef std::map<LOG_LEVEL, std::string> LogPrefixMap;
  LogPrefixMap mPrefixMap;

  typedef std::set<std::string> DefaultSet;
  DefaultSet mDefaultSet;
};

class LoggerFactory {
 public:
  LoggerFactory();
  virtual ~LoggerFactory();

  static int init(const std::string &logFile, Log **logger,
                  LOG_LEVEL logLevel = LOG_LEVEL_INFO,
                  LOG_LEVEL consoleLevel = LOG_LEVEL_WARN,
                  LOG_ROTATE rotateType = LOG_ROTATE_BYDAY);

  static int initDefault(const std::string &logFile,
                         LOG_LEVEL logLevel = LOG_LEVEL_INFO,
                         LOG_LEVEL consoleLevel = LOG_LEVEL_WARN,
                         LOG_ROTATE rotateType = LOG_ROTATE_BYDAY);
};

extern Log *gLog;

#define LOG_HEAD(prefix, level)                                                \
  if (common::gLog) {                                                          \
    time_t now_time;                                                           \
    time(&now_time);                                                           \
    struct tm *p = localtime(&now_time);                                       \
    char szHead[64] = {0};                                                     \
    if (p) {                                                                   \
      sprintf(szHead, "%d-%d-%d %d:%d:%u pid:%u tid:%llx ", p->tm_year + 1900, \
              p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,     \
              (u32_t)getpid(), gettid());                                      \
      common::gLog->Rotate(p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);      \
    }                                                                          \
    snprintf(prefix, sizeof(prefix), "[%s %s %s %s %u]>>", szHead,             \
            (common::gLog)->PrefixMsg(level), __FILE__,                        \
             __FUNCTION__, (u32_t)__LINE__);                                   \
  }

#define LOG_OUTPUT(level, fmt, ...)                                            \
  do {                                                                         \
    using namespace common;                                                    \
    if (gLog && gLog->CheckOutput(level, __FILE__)) {                          \
      char prefix[ONE_KILO] = {0};                                             \
      LOG_HEAD(prefix, level);                                                 \
      gLog->Output(level, __FILE__, prefix, fmt, ##__VA_ARGS__);               \
    }                                                                          \
  } while (0)

#define LOG_DEFAULT(fmt, ...)                                                  \
  LOG_OUTPUT(common::gLog->GetLogLevel(), fmt, ##__VA_ARGS__)
#define LOG_PANIC(fmt, ...) LOG_OUTPUT(common::LOG_LEVEL_PANIC, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_OUTPUT(common::LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) LOG_OUTPUT(common::LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG_OUTPUT(common::LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_OUTPUT(common::LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt, ...) LOG_OUTPUT(common::LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

template<class T>
Log &Log::operator<<(T msg) {
  // at this time, the input level is the default log level
  Out(mConsoleLevel, mLogLevel, msg);
  return *this;
}

template<class T>
int Log::Panic(T message) {
  return Out(LOG_LEVEL_PANIC, LOG_LEVEL_PANIC, message);
}

template<class T>
int Log::Error(T message) {
  return Out(LOG_LEVEL_ERR, LOG_LEVEL_ERR, message);
}

template<class T>
int Log::Warnning(T message) {
  return Out(LOG_LEVEL_WARN, LOG_LEVEL_WARN, message);
}

template<class T>
int Log::Info(T message) {
  return Out(LOG_LEVEL_INFO, LOG_LEVEL_INFO, message);
}

template<class T>
int Log::Debug(T message) {
  return Out(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, message);
}

template<class T>
int Log::Trace(T message) {
  return Out(LOG_LEVEL_TRACE, LOG_LEVEL_TRACE, message);
}

template<class T>
int Log::Out(const LOG_LEVEL consoleLevel, const LOG_LEVEL logLevel, T &msg) {
  bool locked = false;
  if (consoleLevel < LOG_LEVEL_PANIC || consoleLevel > mConsoleLevel ||
    logLevel < LOG_LEVEL_PANIC || logLevel > mLogLevel) {
    return LOG_STATUS_OK;
  }
  try {
    char prefix[ONE_KILO] = {0};
    LOG_HEAD(prefix, logLevel);
    if (LOG_LEVEL_PANIC <= consoleLevel && consoleLevel <= mConsoleLevel) {
      std::cout << mPrefixMap[consoleLevel] << msg;
    }

    if (LOG_LEVEL_PANIC <= logLevel && logLevel <= mLogLevel) {
      pthread_mutex_lock(&mLock);
      locked = true;
      mOfs << prefix;
      mOfs << msg;
      mOfs.flush();
      mLogLine++;
      pthread_mutex_unlock(&mLock);
      locked = false;
    }
  } catch (std::exception &e) {
    if (locked) {
      pthread_mutex_unlock(&mLock);
    }
    std::cerr << e.what() << std::endl;
    return LOG_STATUS_ERR;
  }

  return LOG_STATUS_OK;
}

#ifndef ASSERT
#define ASSERT(expression, description, ...)                                   \
  do {                                                                         \
    if (!(expression)) {                                                       \
      if (common::gLog) {                                                      \
        LOG_PANIC(description, ##__VA_ARGS__);                                 \
        LOG_PANIC("\n");                                                       \
      }                                                                        \
      assert(expression);                                                      \
    }                                                                          \
  } while (0)
#endif // ASSERT

#define SYS_OUTPUT_FILE_POS                                                    \
  ", File:" << __FILE__ << ", line:" << __LINE__ << ",function:" << __FUNCTION__
#define SYS_OUTPUT_ERROR ",error:" << errno << ":" << strerror(errno)

} //namespace common
#endif //__COMMON_LOG_LOG_H__
