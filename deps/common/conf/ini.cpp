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

#include <errno.h>
#include <fstream>
#include <string.h>

#include "common/conf/ini.h"
#include "common/defs.h"
#include "common/lang/string.h"
#include "common/log/log.h"

namespace common {

const std::string Ini::DEFAULT_SECTION = std::string("");
const std::map<std::string, std::string> Ini::mEmptyMap;

Ini::Ini() {}

Ini::~Ini() {}

void Ini::insertSession(const std::string &sessionName) {
  std::map<std::string, std::string> sessionMap;
  std::pair < std::string, std::map < std::string, std::string >> entry =
                                                     std::pair < std::string, std::map < std::string, std::string
    >> (sessionName,
      sessionMap);

  mSections.insert(entry);
}

std::map<std::string, std::string> *Ini::switchSession(
  const std::string &sessionName) {
  SessionsMap::iterator it = mSections.find(sessionName);
  if (it != mSections.end()) {
    return &it->second;
  }

  insertSession(sessionName);

  it = mSections.find(sessionName);
  if (it != mSections.end()) {
    return &it->second;
  }

  // should never go this
  return nullptr;
}

const std::map<std::string, std::string> &Ini::get(
  const std::string &section) {
  SessionsMap::iterator it = mSections.find(section);
  if (it == mSections.end()) {
    return mEmptyMap;
  }

  return it->second;
}

std::string Ini::get(const std::string &key, const std::string &defaultValue,
                     const std::string &section) {
  std::map<std::string, std::string> sectionMap = get(section);

  std::map<std::string, std::string>::iterator it = sectionMap.find(key);
  if (it == sectionMap.end()) {
    return defaultValue;
  }

  return it->second;
}

int Ini::put(const std::string &key, const std::string &value,
             const std::string &section) {
  std::map<std::string, std::string> *pSectionMap = switchSession(section);

  pSectionMap->insert(std::pair<std::string, std::string>(key, value));

  return 0;
}

int Ini::insertEntry(std::map<std::string, std::string> *sessionMap,
                     const std::string &line) {
  if (sessionMap == nullptr) {
    std::cerr << __FILE__ << __FUNCTION__ << " session map is null"
              << std::endl;
    return -1;
  }
  size_t equalPos = line.find_first_of('=');
  if (equalPos == std::string::npos) {
    std::cerr << __FILE__ << __FUNCTION__ << "Invalid configuration line "
              << line << std::endl;
    return -1;
  }

  std::string key = line.substr(0, equalPos);
  std::string value = line.substr(equalPos + 1);

  strip(key);
  strip(value);

  sessionMap->insert(std::pair<std::string, std::string>(key, value));

  return 0;
}

int Ini::load(const std::string &fileName) {
  std::ifstream ifs;

  try {

    bool continueLastLine = false;

    std::map<std::string, std::string> *currentSession =
      switchSession(DEFAULT_SECTION);

    char line[MAX_CFG_LINE_LEN];

    std::string lineEntry;

    ifs.open(fileName.c_str());
    while (ifs.good()) {

      memset(line, 0, sizeof(line));

      ifs.getline(line, sizeof(line));

      char *readBuf = strip(line);

      if (strlen(readBuf) == 0) {
        // empty line
        continue;
      }

      if (readBuf[0] == CFG_COMMENT_TAG) {
        // comments line
        continue;
      }

      if (readBuf[0] == CFG_SESSION_START_TAG &&
        readBuf[strlen(readBuf) - 1] == CFG_SESSION_END_TAG) {

        readBuf[strlen(readBuf) - 1] = '\0';
        std::string sessionName = std::string(readBuf + 1);

        currentSession = switchSession(sessionName);

        continue;
      }

      if (continueLastLine == false) {
        // don't need continue last line
        lineEntry = readBuf;
      } else {
        lineEntry += readBuf;
      }

      if (readBuf[strlen(readBuf) - 1] == CFG_CONTINUE_TAG) {
        // this line isn't finished, need continue
        continueLastLine = true;

        // remove the last character
        lineEntry = lineEntry.substr(0, lineEntry.size() -1);
        continue;
      } else {
        continueLastLine = false;
        insertEntry(currentSession, lineEntry);
      }
    }
    ifs.close();

    mFileNames.insert(fileName);
    std::cout << "Successfully load " << fileName << std::endl;
  } catch (...) {
    if (ifs.is_open()) {
      ifs.close();
    }
    std::cerr << "Failed to load " << fileName << SYS_OUTPUT_ERROR
              << std::endl;
    return -1;
  }

  return 0;
}

void Ini::toString(std::string &outputStr) {
  outputStr.clear();

  outputStr += "Begin dump configuration\n";

  for (SessionsMap::iterator it = mSections.begin(); it != mSections.end();
       it++) {
    outputStr += CFG_SESSION_START_TAG;
    outputStr += it->first;
    outputStr += CFG_SESSION_END_TAG;
    outputStr += "\n";

    std::map<std::string, std::string> &sectionMap = it->second;

    for (std::map<std::string, std::string>::iterator subIt =
      sectionMap.begin();
         subIt != sectionMap.end(); subIt++) {
      outputStr += subIt->first;
      outputStr += "=";
      outputStr += subIt->second;
      outputStr += "\n";
    }
    outputStr += "\n";
  }

  outputStr += "Finish dump configuration \n";

  return;
}

//! Accessor function which wraps global properties object
Ini *&theGlobalProperties() {
  static Ini *gProperties = new Ini();
  return gProperties;
}

} //namespace common