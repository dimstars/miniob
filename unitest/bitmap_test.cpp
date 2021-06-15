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
// Created by wangyunlai.wyl on 2021
//

#include <string.h>

#include "gtest/gtest.h"
#include "common/lang/bitmap.h"
#include <sstream>

using namespace common;

TEST(test_bitmap, test_bitmap) {
  char buf1[1];
  memset(buf1, 0, sizeof(buf1));
  Bitmap bitmap(buf1, 8);

  for (int i = 0; i < 8; i++) {
    ASSERT_EQ(bitmap.get_bit(i), false);
  }

  ASSERT_EQ(0, bitmap.next_unsetted_bit(0));
  ASSERT_EQ(-1, bitmap.next_setted_bit(0));

  for (int i = 0; i < 8; i++) {
    bitmap.set_bit(i);
    ASSERT_EQ(bitmap.get_bit(i), true);
  }
  ASSERT_EQ(-1, bitmap.next_unsetted_bit(0));
  ASSERT_EQ(0, bitmap.next_setted_bit(0));

  for (int i = 0; i < 8; i++) {
    bitmap.clear_bit(i);
  }
  for (int i = 0; i < 8; i++) {
    ASSERT_EQ(bitmap.get_bit(i), false);
  }

  char buf3[3];
  memset(buf3, 0, sizeof(buf3));

  Bitmap bitmap3(buf3, 22);
  ASSERT_EQ(0, bitmap3.next_unsetted_bit(0));
  ASSERT_EQ(21, bitmap3.next_unsetted_bit(21));
  ASSERT_EQ(-1, bitmap3.next_unsetted_bit(22));

  buf3[0] = -1;
  buf3[1] = -1;
  buf3[2] = -1;
  ASSERT_EQ(0, bitmap3.next_setted_bit(0));
  ASSERT_EQ(21, bitmap3.next_setted_bit(21));
  ASSERT_EQ(-1, bitmap3.next_setted_bit(22));

  buf3[1] = 0;
  ASSERT_EQ(8, bitmap3.next_unsetted_bit(0));
  ASSERT_EQ(16, bitmap3.next_setted_bit(8));
}

int main(int argc, char **argv) {
  // 分析gtest程序的命令行参数
  testing::InitGoogleTest(&argc, argv);

  // 调用RUN_ALL_TESTS()运行所有测试用例
  // main函数返回RUN_ALL_TESTS()的运行结果
  return RUN_ALL_TESTS();
}