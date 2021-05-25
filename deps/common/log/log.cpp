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

#include <assert.h>
#include <exception>
#include <stdarg.h>
#include <stdio.h>

#include "common/lang/string.h"
#include "common/log/log.h"
namespace common {

Log *gLog = nullptr;

Log::Log(const std::string &logFileName, const LOG_LEVEL logLevel,
         const LOG_LEVEL consoleLevel)
    : mLogName(logFileName), mLogLevel(logLevel), mConsoleLevel(consoleLevel) {
  mPrefixMap[LOG_LEVEL_PANIC] = "PANIC:";
  mPrefixMap[LOG_LEVEL_ERR] = "ERROR:";
  mPrefixMap[LOG_LEVEL_WARN] = "WARNNING:";
  mPrefixMap[LOG_LEVEL_INFO] = "INFO:";
  mPrefixMap[LOG_LEVEL_DEBUG] = "DEBUG:";
  mPrefixMap[LOG_LEVEL_TRACE] = "TRACE:";

  pthread_mutex_init(&mLock, nullptr);

  mLogDate.mYear = -1;
  mLogDate.mMon = -1;
  mLogDate.mDay = -1;
  mLogMaxLine = LOG_MAX_LINE;
  mLogLine = -1;
  mRotateType = LOG_ROTATE_BYDAY;

  CheckParamValid();
}

Log::~Log(void) {
  pthread_mutex_lock(&mLock);
  if (mOfs.is_open()) {
    mOfs.close();
  }
  pthread_mutex_unlock(&mLock);

  pthread_mutex_destroy(&mLock);
}

void Log::CheckParamValid() {
  assert(!mLogName.empty());
  assert(LOG_LEVEL_PANIC <= mLogLevel && mLogLevel < LOG_LEVEL_LAST);
  assert(LOG_LEVEL_PANIC <= mConsoleLevel && mConsoleLevel < LOG_LEVEL_LAST);

  return;
}

bool Log::CheckOutput(const LOG_LEVEL level, const char *module) {
  if (LOG_LEVEL_LAST > level && level <= mConsoleLevel) {
    return true;
  }
  if (LOG_LEVEL_LAST > level && level <= mLogLevel) {
    return true;
  }
  // in order to improve speed
  if (mDefaultSet.empty() == false && mDefaultSet.find(module) != mDefaultSet.end()) {
    return true;
  }
  return false;
}

int Log::Output(const LOG_LEVEL level, const char *module, const char *prefix,
                const char *f, ...) {
  bool locked = false;
  try {
    va_list args;
    char msg[ONE_KILO];

    va_start(args, f);
    vsnprintf(msg, sizeof(msg), f, args);
    va_end(args);

    if (LOG_LEVEL_PANIC <= level && level <= mConsoleLevel) {
      std::cout << msg << std::endl;
    } else if (mDefaultSet.find(module) != mDefaultSet.end()) {
      std::cout << msg << std::endl;
    }

    if (LOG_LEVEL_PANIC <= level && level <= mLogLevel) {
      pthread_mutex_lock(&mLock);
      locked = true;
      mOfs << prefix;
      mOfs << msg;
      mOfs << "\n";
      mOfs.flush();
      mLogLine++;
      pthread_mutex_unlock(&mLock);
      locked = false;
    } else if (mDefaultSet.find(module) != mDefaultSet.end()) {
      pthread_mutex_lock(&mLock);
      locked = true;
      mOfs << prefix;
      mOfs << msg;
      mOfs << "\n";
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

int Log::SetConsoleLevel(LOG_LEVEL consoleLevel) {
  if (LOG_LEVEL_PANIC <= consoleLevel && consoleLevel < LOG_LEVEL_LAST) {
    mConsoleLevel = consoleLevel;
    return LOG_STATUS_OK;
  }

  return LOG_STATUS_ERR;
}

LOG_LEVEL Log::GetConsoleLevel() { return mConsoleLevel; }

int Log::SetLogLevel(LOG_LEVEL logLevel) {
  if (LOG_LEVEL_PANIC <= logLevel && logLevel < LOG_LEVEL_LAST) {
    mLogLevel = logLevel;
    return LOG_STATUS_OK;
  }

  return LOG_STATUS_ERR;
}

LOG_LEVEL Log::GetLogLevel() { return mLogLevel; }

const char *Log::PrefixMsg(LOG_LEVEL level) {
  if (LOG_LEVEL_PANIC <= level && level < LOG_LEVEL_LAST) {
    return mPrefixMap[level].c_str();
  }
  static const char *emptyPrefix = "";
  return emptyPrefix;
}

void Log::SetDefaultModule(const std::string &modules) {
  splitString(modules, ",", mDefaultSet);
}

int Log::SetRotateType(LOG_ROTATE rotateType) {
  if (LOG_ROTATE_BYDAY <= rotateType && rotateType < LOG_ROTATE_LAST) {
    mRotateType = rotateType;
  }
  return LOG_STATUS_OK;
}

LOG_ROTATE Log::GetRotateType() { return mRotateType; }

int Log::RotateByDay(const int year, const int month, const int day) {
  if (mLogDate.mYear == year && mLogDate.mMon == month &&
      mLogDate.mDay == day) {
    // Don't need rotate
    return 0;
  }

  char date[16] = {0};
  snprintf(date, sizeof(date), "%04d%02d%02d", year, month, day);
  std::string logFileName = mLogName + "." + date;

  if (mOfs.is_open()) {
    mOfs.close();
  }
  mOfs.open(logFileName.c_str(), std::ios_base::out | std::ios_base::app);
  if (mOfs.good()) {
    mLogDate.mYear = year;
    mLogDate.mMon = month;
    mLogDate.mDay = day;

    mLogLine = 0;
  }

  return 0;
}

int Log::RenameOldLogs() {
  int logIndex = 1;
  int maxLogIndex = 0;
  char logIndexStr[4] = {0};

  while (logIndex < 999) {
    snprintf(logIndexStr, sizeof(logIndexStr), "%03d", logIndex);

    std::string logName = mLogName + "." + logIndexStr;
    int result = access(logName.c_str(), R_OK);
    if (result) {
      break;
    }

    maxLogIndex = logIndex;
    logIndex++;
  }

  logIndex = maxLogIndex;
  while (logIndex > 0) {
    snprintf(logIndexStr, sizeof(logIndexStr), "%03d", logIndex);

    std::string logNameOld = mLogName + "." + logIndexStr;

    snprintf(logIndexStr, sizeof(logIndexStr), "%03d", logIndex + 1);

    std::string logNameNew = mLogName + "." + logIndexStr;

    int result = rename(logNameOld.c_str(), logNameNew.c_str());
    if (result) {
      return LOG_STATUS_ERR;
    }
    logIndex--;
  }

  return LOG_STATUS_OK;
}

int Log::RotateBySize() {
  if (mLogLine < 0) {
    // The first time open log file
    mOfs.open(mLogName.c_str(), std::ios_base::out | std::ios_base::app);
    mLogLine = 0;
    return LOG_STATUS_OK;
  } else if (0 <= mLogLine && mLogLine < mLogMaxLine) {
    // Don't need rotate
    return LOG_STATUS_OK;
  } else {

    int result = RenameOldLogs();
    if (result) {
      // rename old logs failed
      return LOG_STATUS_OK;
    }

    if (mOfs.is_open()) {
      mOfs.close();
    }

    char logIndexStr[4] = {0};
    snprintf(logIndexStr, sizeof(logIndexStr), "%03d", 1);
    std::string logNameNew = mLogName + "." + logIndexStr;
    result = rename(mLogName.c_str(), logNameNew.c_str());
    if (result) {
      std::cerr << "Failed to rename " << mLogName << " to " << logNameNew
                << std::endl;
    }

    mOfs.open(mLogName.c_str(), std::ios_base::out | std::ios_base::app);
    if (mOfs.good()) {
      mLogLine = 0;
    } else {
      // Error
      mLogLine = mLogMaxLine;
    }

    return LOG_STATUS_OK;
  }

  return LOG_STATUS_OK;
}

int Log::Rotate(const int year, const int month, const int day) {
  int result = 0;
  pthread_mutex_lock(&mLock);
  if (mRotateType == LOG_ROTATE_BYDAY) {
    result = RotateByDay(year, month, day);
  } else {
    result = RotateBySize();
  }
  pthread_mutex_unlock(&mLock);

  return result;
}

LoggerFactory::LoggerFactory() {
  // TODO Auto-generated constructor stub
}

LoggerFactory::~LoggerFactory() {
  // TODO Auto-generated destructor stub
}

int LoggerFactory::init(const std::string &logFile, Log **logger,
                        LOG_LEVEL logLevel, LOG_LEVEL consoleLevel,
                        LOG_ROTATE rotateType) {
  Log *log = new (std::nothrow) Log(logFile, logLevel, consoleLevel);
  if (log == nullptr) {
    std::cout << "Error" << std::endl;
    return -1;
  }
  log->SetRotateType(rotateType);

  *logger = log;

  return 0;
}

int LoggerFactory::initDefault(const std::string &logFile, LOG_LEVEL logLevel,
                               LOG_LEVEL consoleLevel, LOG_ROTATE rotateType) {
  if (gLog != nullptr) {
    LOG_WARN("Default logger has been initialized");
    return 0;
  }

  return init(logFile, &gLog, logLevel, consoleLevel, rotateType);
}

} //namespace common