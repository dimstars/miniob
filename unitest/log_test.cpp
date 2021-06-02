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


#include "log_test.h"


#include "gtest/gtest.h"

#include "common/log/log.h"


using namespace common;

LogTest::LogTest() {
  // TODO Auto-generated constructor stub
}

LogTest::~LogTest() {
  // TODO Auto-generated destructor stub
}

int LogTest::init(const std::string &logFile) {



  LoggerFactory::init_default(logFile);

  g_log->set_rotate_type(LOG_ROTATE_BYSIZE);

  return 0;
}

void *LogTest::log_loop(void *param) {
  int index = *(int *) param;
  int i = 0;
  while (i < 100) {
    i++;
    LOG_INFO("index:%d --> %d", index, i);
  }

  return NULL;
}

void checkRotate() {
  LogTest test;

  test.init();
  ASSERT_EQ(g_log->get_rotate_type(), LOG_ROTATE_BYSIZE);

  int index = 30;
  test.log_loop(&index);
}

TEST(checkRotateTest, CheckRoateTest)
{

}

void testEnableTest() {
  LogTest test;

  test.init();


  ASSERT_EQ(g_log->check_output(LOG_LEVEL_PANIC, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_ERR, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_WARN, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_INFO, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_DEBUG, __FILE__), false);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_TRACE, __FILE__), false);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_LAST, __FILE__), false);

  g_log->set_default_module(__FILE__);

  ASSERT_EQ(g_log->check_output(LOG_LEVEL_PANIC, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_ERR, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_WARN, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_INFO, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_DEBUG, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_TRACE, __FILE__), true);
  ASSERT_EQ(g_log->check_output(LOG_LEVEL_LAST, __FILE__), true);
}

TEST(testEnableTest, CheckEnableTest)
{

}

int main(int argc, char **argv) {


  // 分析gtest程序的命令行参数
  testing::InitGoogleTest(&argc, argv);

  // 调用RUN_ALL_TESTS()运行所有测试用例
  // main函数返回RUN_ALL_TESTS()的运行结果
  return RUN_ALL_TESTS();
}
