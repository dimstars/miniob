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

#include <stdio.h>

#include "common/math/md5.h"
#include "md5_test.h"

using namespace common;

Md5Test::Md5Test() {
  // TODO Auto-generated constructor stub
}

Md5Test::~Md5Test() {
  // TODO Auto-generated destructor stub
}

void Md5Test::string() {
  char buf[512] = "/home/fastdfs/longda";
  unsigned char digest[16] = {0};
  MD5String(buf, digest);
  for (int i = 0; i < 16; i++) {
    printf("%d: %02x %d\n", i, digest[i], digest[i]);
  }
}

int main(int argc, char **argv) {
  Md5Test test;
  test.string();

  return 0;
}
