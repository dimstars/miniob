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

#ifndef __COMMON_IO_ROLL_SELECT_DIR__
#define __COMMON_IO_ROLL_SELECT_DIR__

#include <map>
#include <string>
#include <vector>

#include "common/defs.h"
#include "common/io/select_dir.h"
#include "common/lang/mutex.h"
namespace common {

class RollSelectDir : public SelectDir {
public:
  RollSelectDir() { MUTEX_INIT(&mMutex, NULL); }
  ~RollSelectDir() { MUTEX_DESTROY(&mMutex); }

public:
  /**
   * inherit from CSelectDir
   */
  std::string select();
  void setBaseDir(std::string baseDir);

public:
  std::string mBaseDir;
  std::vector<std::string> mSubdirs;
  pthread_mutex_t mMutex;
  u32_t mPos;
};

} //namespace common
#endif /* __COMMON_IO_ROLL_SELECT_DIR__ */
