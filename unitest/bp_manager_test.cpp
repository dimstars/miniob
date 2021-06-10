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

#include "storage/default/disk_buffer_pool.h"
#include "gtest/gtest.h"

TEST(test_bp_manager, test_bp_manager_simple_lru) {
  BPManager bp_manager(2);

  Frame * frame1 = bp_manager.alloc();
  ASSERT_NE(frame1, nullptr);

  frame1->file_desc = 0;
  frame1->page.page_num = 1;

  ASSERT_EQ(frame1, bp_manager.get(0, 1));

  Frame *frame2 = bp_manager.alloc();
  ASSERT_NE(frame2, nullptr);
  frame2->file_desc = 0;
  frame2->page.page_num = 2;

  ASSERT_EQ(frame1, bp_manager.get(0, 1));

  Frame *frame3 = bp_manager.alloc();
  ASSERT_NE(frame3, nullptr);
  frame3->file_desc = 0;
  frame3->page.page_num = 3;

  frame2 = bp_manager.get(0, 2);
  ASSERT_EQ(frame2, nullptr);

  Frame *frame4 = bp_manager.alloc();
  frame4->file_desc = 0;
  frame4->page.page_num = 4;

  frame1 = bp_manager.get(0, 1);
  ASSERT_EQ(frame1, nullptr);

  frame3 = bp_manager.get(0, 3);
  ASSERT_NE(frame3, nullptr);

  frame4 = bp_manager.get(0, 4);
  ASSERT_NE(frame4, nullptr);
}

int main(int argc, char **argv) {


  // 分析gtest程序的命令行参数
  testing::InitGoogleTest(&argc, argv);

  // 调用RUN_ALL_TESTS()运行所有测试用例
  // main函数返回RUN_ALL_TESTS()的运行结果
  return RUN_ALL_TESTS();
}