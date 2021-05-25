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
// Created by Longda on 2021/4/16.
//
#include <unistd.h>

#include "gtest/gtest.h"

#include "common/os/pidfile.h"
#include "common/io/io.h"
#include "common/lang/string.h"

using namespace common;

int main() {
  long long pid = (long long)getpid();

  const char *programName = "test";
  writePidFile(programName);

  std::string pidFile = getPidPath();

  char buf[1024] = {0};
  char *p = buf;
  size_t size = 0;
  readFromFile(pidFile, p, size);

  std::string temp(p);
  long long target = 0;
  strToVal(temp, target);

  EXPECT_EQ(pid, target);
}