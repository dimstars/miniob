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

#include "common/io/roll_select_dir.h"
#include "common/io/io.h"
#include "common/log/log.h"
namespace common {

void RollSelectDir::setBaseDir(std::string baseDir) {
  mBaseDir = baseDir;

  std::vector<std::string> dirList;
  int rc = getDirList(dirList, mBaseDir, "");
  if (rc) {
    LOG_ERROR("Failed to all subdir entry");
  }

  if (dirList.size() == 0) {
    MUTEX_LOCK(&mMutex);

    mSubdirs.clear();
    mSubdirs.push_back(mBaseDir);
    mPos = 0;
    MUTEX_UNLOCK(&mMutex);

    return;
  }

  MUTEX_LOCK(&mMutex);
  mSubdirs = dirList;
  mPos = 0;
  MUTEX_UNLOCK(&mMutex);
  return;
}

std::string RollSelectDir::select() {
  std::string ret;

  MUTEX_LOCK(&mMutex);
  ret = mSubdirs[mPos % mSubdirs.size()];
  mPos++;
  MUTEX_UNLOCK(&mMutex);

  return ret;
}

} //namespace common