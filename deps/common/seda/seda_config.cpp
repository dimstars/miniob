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
#include <sys/stat.h>
#include <sys/types.h>
#include <typeinfo>
#include <unistd.h>
#include <vector>

#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/os/os.h"

#include "common/seda/init.h"
#include "common/seda/seda_config.h"
#include "common/seda/stage_factory.h"
namespace common {

SedaConfig *SedaConfig::instance = NULL;



SedaConfig *&SedaConfig::getInstance() {
  if (instance == NULL) {
    instance = new SedaConfig();
    ASSERT((instance != NULL), "failed to allocate SedaConfig");
  }
  return instance;
}

//! Constructor
SedaConfig::SedaConfig() : mCfgFile(), mCfgStr(), mThreadPools(), mStages() {
  return;
}

//! Destructor
SedaConfig::~SedaConfig() {
  ASSERT(instance, "Instance should not be null");
  // check to see if clean-up is necessary
  if ((!mThreadPools.empty()) || (!mStages.empty())) {
    cleanup();
  }

  instance = NULL;
}

//! Set the file holding the configuration
void SedaConfig::setCfgFilename(const char *filename) {
  mCfgStr.clear();
  mCfgFile.clear();
  if (filename != NULL) {
    mCfgFile.assign(filename);
  }
  return;
}

//! Set the string holding the configuration
void SedaConfig::setCfgString(const char *configStr) {
  mCfgStr.clear();
  mCfgFile.clear();
  if (configStr != NULL) {
    mCfgStr.assign(configStr);
  }
  return;
}

//! Parse config file or string
SedaConfig::status_t SedaConfig::parse() {
  // first parse the config
  try {
    // skip parse in this implementation
    // all configuration will be put into one file
  } catch (const std::exception &e) {
    std::string errMsg(e.what());
    LOG_ERROR("Seda config parse failed w/ error %s", errMsg.c_str());
    return PARSEFAIL;
  }
  LOG_DEBUG("Seda config parse success\n");

  return SUCCESS;
}

//! instantiate the parsed SEDA configuration
SedaConfig::status_t SedaConfig::instantiateCfg() {
  status_t stat = SUCCESS;

  // instantiate the configuration
  stat = instantiate();

  return stat;
}

//! start the configuration - puts the mStages into action
SedaConfig::status_t SedaConfig::start() {
  status_t stat = SUCCESS;

  ASSERT(mThreadPools.size(), "Configuration not yet instantiated");

  // start the mStages one by one.  connect() calls
  std::map<std::string, Stage *>::iterator iter = mStages.begin();
  std::map<std::string, Stage *>::iterator end = mStages.end();

  while (iter != end) {
    if (iter->second != NULL) {
      Stage *stg = iter->second;
      bool ret = stg->connect();
      if (!ret) {
        cleanup();
        stat = INITFAIL;
        break;
      }
    }
    iter++;
  }

  return stat;
}

//! Initialize the mThreadPools and mStages
SedaConfig::status_t SedaConfig::init() {
  status_t stat = SUCCESS;

  // check the preconditions
  ASSERT(mStages.empty(), "Attempt to initialize sedaconfig twice");
  ASSERT(mThreadPools.empty(), "Attempt to initialize sedaconfig twice");

  // instantiate the parsed config
  stat = instantiate();
  if (stat) {
    return stat;
  }

  // start it running
  stat = start();
  if (stat) {
    return stat;
  }

  return SUCCESS;
}

//! Clean-up the threadpool and mStages
void SedaConfig::cleanup() {
  // first disconnect all mStages
  if (mStages.empty() == false) {
    std::map<std::string, Stage *>::iterator iter = mStages.begin();
    std::map<std::string, Stage *>::iterator end = mStages.end();
    while (iter != end) {
      if (iter->second != NULL) {
        Stage *stg = iter->second;
        if (stg->isConnected()) {
          stg->disconnect();
        }
      }
      iter++;
    }
  }
  LOG_TRACE("mStages disconnected\n");

  // now delete all mStages and mThreadPools
  clearConfig();
}

void SedaConfig::initEventHistory() {
  std::map<std::string, std::string> baseSection =
    theGlobalProperties()->get(SEDA_BASE_NAME);
  std::map<std::string, std::string>::iterator it;
  std::string key;

  // check whether event histories are enabled
  bool evHist = false;
  key = EVENT_HISTORY;
  it = baseSection.find(key);
  if (it != baseSection.end()) {
    if (it->second.compare("true") == 0) {
      evHist = true;
    }
  }

  theEventHistoryFlag() = evHist;

  // set max event hops
  u32_t maxEventHops = 100;
  key = MAX_EVENT_HISTORY_NUM;
  it = baseSection.find(key);
  if (it != baseSection.end()) {
    strToVal(it->second, maxEventHops);
  }
  theMaxEventHops() = maxEventHops;

  LOG_INFO("Successfully initEventHistory, EventHistory:%d, MaxEventHops:%u",
           (int) evHist, maxEventHops);
  return;
}

SedaConfig::status_t SedaConfig::initThreadPool() {
  try {

    std::map<std::string, std::string> baseSection =
      theGlobalProperties()->get(SEDA_BASE_NAME);
    std::map<std::string, std::string>::iterator it;
    std::string key;

    // get thread pool names
    key = THREAD_POOLS_NAME;
    it = baseSection.find(key);
    if (it == baseSection.end()) {
      LOG_ERROR("Configuration hasn't set %s", key.c_str());
      return INITFAIL;
    }

    std::string poolNames = it->second;
    std::vector<std::string> nameList;
    std::string splitTag;
    splitTag.assign(1, Ini::CFG_DELIMIT_TAG);
    splitString(poolNames, splitTag, nameList);

    int cpuNum = getCpuNum();
    std::string defaultCpuNumStr;
    valToStr(cpuNum, defaultCpuNumStr);

    for (size_t pos = 0; pos != nameList.size(); pos++) {
      std::string &threadName = nameList[pos];

      // get count number
      key = COUNT;
      std::string countStr = theGlobalProperties()->get(key, defaultCpuNumStr, threadName);

      int threadCount = 1;
      strToVal(countStr, threadCount);
      if (threadCount < 1) {
        LOG_INFO("Thread number of  %s is %d, it is same as cpu's cores.",
                  threadName.c_str(), cpuNum);
        threadCount = cpuNum;
      }

      mThreadPools[threadName] = new Threadpool(threadCount, threadName);
      if (mThreadPools[threadName] == NULL) {
        LOG_ERROR("Failed to new %s threadpool\n", threadName.c_str());
        return INITFAIL;
      }
    }

    if  (mThreadPools.find(DEFAULT_THREAD_POOL) == mThreadPools.end()) {
      LOG_ERROR("There is no default thread pool %s, please add it.",
                DEFAULT_THREAD_POOL);
      return INITFAIL;
    }

  } catch (std::exception &e) {
    LOG_ERROR("Failed to init mThreadPools:%s\n", e.what());
    clearConfig();
    return INITFAIL;
  }

  int nPools = mThreadPools.size();
  if (nPools < 1) {
    LOG_ERROR("Invalid number of mThreadPools:%d", nPools);
    clearConfig();
    return INITFAIL;
  }

  return SUCCESS;
}

std::string SedaConfig::getThreadPool(std::string &stageName) {
  std::string ret = DEFAULT_THREAD_POOL;
  // Get thread pool
  std::map<std::string, std::string> stageSection =
      theGlobalProperties()->get(stageName);
  std::map<std::string, std::string>::iterator itt;
  std::string threadPoolId = THREAD_POOL_ID;
  itt = stageSection.find(threadPoolId);
  if (itt == stageSection.end()) {
    LOG_INFO("Not set threadPoolId for %s, use default threadpool %s",
             stageName.c_str(), DEFAULT_THREAD_POOL);
    return ret;
  }

  std::string threadName = itt->second;
  if (threadName.empty()) {
    LOG_ERROR("Failed to set %s of the %s, use the default threadpool %s",
              threadPoolId.c_str(), stageName.c_str(), DEFAULT_THREAD_POOL);
    return ret;
  }

  if (mThreadPools.find(threadName) == mThreadPools.end()) {
    LOG_ERROR("The stage %s's threadpool %s is invalid, use the default "
              "threadpool %s",
              stageName.c_str(), threadName.c_str(), DEFAULT_THREAD_POOL);
  }
  return ret;
}

SedaConfig::status_t SedaConfig::initStages() {
  try {
    std::map<std::string, std::string> baseSection =
      theGlobalProperties()->get(SEDA_BASE_NAME);
    std::map<std::string, std::string>::iterator it;
    std::string key;

    // get stage names
    key = STAGES;
    it = baseSection.find(key);
    if (it == baseSection.end()) {
      LOG_ERROR("Hasn't set stages name in %s", key.c_str());
      clearConfig();
      return INITFAIL;
    }

    std::string splitTag;
    splitTag.assign(1, Ini::CFG_DELIMIT_TAG);
    splitString(it->second, splitTag, mStageNames);

    for (std::vector<std::string>::iterator it = mStageNames.begin();
         it != mStageNames.end(); it++) {
      std::string stageName(*it);

      std::string threadName = getThreadPool(stageName);
      Threadpool *t = mThreadPools[threadName];

      Stage *stage = StageFactory::makeInstance(stageName);
      if (stage == NULL) {
        LOG_ERROR("Failed to make instance of stage %s", stageName.c_str());
        clearConfig();
        return INITFAIL;
      }
      mStages[stageName] = stage;
      stage->setPool(t);

      LOG_INFO("Stage %s use threadpool %s.",
               stageName.c_str(), threadName.c_str());
    } // end for stage

  } catch (std::exception &e) {
    LOG_ERROR("Failed to parse stages information, please check, err:%s",
              e.what());
    clearConfig();
    return INITFAIL;
  }

  if (mStages.size() < 1) {
    LOG_ERROR("Invalid number of mStages: %u\n", mStages.size());
    clearConfig();
    return INITFAIL;
  }

  return SUCCESS;
}

SedaConfig::status_t SedaConfig::genNextStages() {
  try {
    for (std::vector<std::string>::iterator itName = mStageNames.begin();
         itName != mStageNames.end(); itName++) {

      std::string stageName(*itName);
      Stage *stage = mStages[stageName];

      std::map<std::string, std::string> stageSection =
        theGlobalProperties()->get(stageName);
      std::map<std::string, std::string>::iterator it;
      std::string nextStageId = NEXT_STAGES;
      it = stageSection.find(nextStageId);
      if (it == stageSection.end()) {
        continue;
      }

      std::string nextStageNames = it->second;

      std::vector<std::string> nextStageNameList;
      std::string splitTag;
      splitTag.assign(1, Ini::CFG_DELIMIT_TAG);
      splitString(nextStageNames, splitTag, nextStageNameList);

      for (std::vector<std::string>::iterator nextIt =
        nextStageNameList.begin();
           nextIt != nextStageNameList.end(); nextIt++) {
        std::string &nextStageName = *nextIt;
        Stage *nextStage = mStages[nextStageName];
        stage->pushStage(nextStage);
      }

    } // end for stage
  } catch (std::exception &e) {
    LOG_ERROR("Failed to get next stages");
    clearConfig();
    return INITFAIL;
  }
  return SUCCESS;
}

//! instantiate the mThreadPools and mStages
SedaConfig::status_t SedaConfig::instantiate() {

  initEventHistory();

  SedaConfig::status_t status = initThreadPool();
  if (status) {
    LOG_ERROR("Failed to init thread pool\n");
    return status;
  }

  status = initStages();
  if (status) {
    LOG_ERROR("Failed init mStages\n");
    return status;
  }

  status = genNextStages();
  if (status) {
    LOG_ERROR("Failed to generate next stage list\n");
    return status;
  }

  return SUCCESS;
}

//! delete all mThreadPools and mStages
void SedaConfig::clearConfig() {
  // delete mStages
  std::map<std::string, Stage *>::iterator s_iter = mStages.begin();
  std::map<std::string, Stage *>::iterator s_end = mStages.end();
  while (s_iter != s_end) {
    if (s_iter->second != NULL) {

      Stage *stg = s_iter->second;
      LOG_INFO("Stage %s deleted.", stg->getName());
      ASSERT((!stg->isConnected()), "%s%s", "Stage connected in clearConfig ",
             stg->getName());
      delete stg;
      s_iter->second = NULL;
    }
    s_iter++;
  }
  mStages.clear();
  LOG_INFO("Seda Stage released");


  // delete mThreadPools
  std::map<std::string, Threadpool *>::iterator t_iter = mThreadPools.begin();
  std::map<std::string, Threadpool *>::iterator t_end = mThreadPools.end();
  while (t_iter != t_end) {
    if (t_iter->second != NULL) {
      LOG_INFO("ThreadPool %s deleted.", t_iter->first.c_str());
      delete t_iter->second;
      t_iter->second = NULL;
    }
    t_iter++;
  }
  mThreadPools.clear();
  LOG_INFO("Seda thread pools released");
}

void SedaConfig::getStageNames(std::vector<std::string> &names) const {
  names = mStageNames;
}

void SedaConfig::getStageQueueStatus(std::vector<int> &stats) const {
  for (std::map<std::string, Stage *>::const_iterator i = mStages.begin();
       i != mStages.end(); ++i) {
    Stage *stg = (*i).second;
    stats.push_back(stg->qlen());
  }
}

//! Global seda config object
SedaConfig *&theSedaConfig() {
  static SedaConfig *sedaConfig = NULL;

  return sedaConfig;
}

} //namespace common