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
// Created by Longda on 2021/4/13.
//

#ifndef __OBSERVER_SQL_EXECUTOR_H_
#define __OBSERVER_SQL_EXECUTOR_H_
#include "handler/handler_defs.h"
#include "rc.h"
// TODO remove this
typedef struct SelResult {
  int col_num;
  int row_num;
  AttrType type[20];
  int length[20];
  char fields[20][20];
  char **res[100];
  SelResult *next_res;
} SelResult;

/**
 * 前六个参数逻辑上可分为三组。
 * 每一组的第一个参数是一个整型n，表示本组中第二个参数的条目数量；
 * 第二个参数是一个包含实际条目数的为n的数组。
 * 例如，参数nSelAttrs包含所选属性的数量，
 * 而参数selAttrs是包含实际属性的长度为nSelAttrs的数组，其余类似。
 * 具体而言，第一组参数表示查询涉及的属性；第二组表示查询涉及的表；第三组表示查询条件。
 * 最后一个参数res用于返回查询结果集
 * @param nSelAttrs
 * @param selAttrs
 * @param nRelations
 * @param relations
 * @param nConditions
 * @param conditions
 * @param res
 * @return
 */
const RC select(int nSelAttrs, RelAttr **selAttrs, int nRelations, char **relations,
          int nConditions, Condition *conditions, SelResult *res);

/**
 * 执行一条SELECT语句，查询结果集通过res参数返回。此函数是提供给测试程序专用的接口
 * @param sql
 * @param res
 * @return
 */
const RC query(char *sql, SelResult *res);

#endif //__OBSERVER_SQL_EXECUTOR_H_