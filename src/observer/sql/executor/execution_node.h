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
// Created by Wangyunlai on 2021/5/13.
//

#ifndef __OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
#define __OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_

#include <vector>
#include "storage/common/condition_filter.h"
#include "sql/executor/tuple.h"

class Table;
class Trx;

class ExecutionNode {
public:
  ExecutionNode() = default;
  virtual ~ExecutionNode() = default;

  virtual RC execute(TupleSet &tuple_set) = 0;
};

class SelectExeNode : public ExecutionNode {
public:
  SelectExeNode();
  virtual ~SelectExeNode() = default;

  RC init(Trx *trx, Table *table, TupleSchema && tuple_schema, std::vector<DefaultConditionFilter> &&condition_filters);

  RC execute(TupleSet &tuple_set) override;
private:
  Trx *trx_ = nullptr;
  Table  * table_;
  TupleSchema  tuple_schema_;
  std::vector<DefaultConditionFilter> condition_filters_;
};

class CompositeTupleConditionFilter;
class TupleExtractor;

class JoinExeNode : public ExecutionNode {
public:
  JoinExeNode() = default;
  virtual ~JoinExeNode();

  RC init(const std::vector<TupleSet> &tuple_set_list, TupleSchema &&tuple_schema,
          std::vector<const Condition *> &&condition_filters);
  RC execute(TupleSet &tuple_set) override;

private:
  const std::vector<TupleSet> *tuple_set_list_ = nullptr;
  TupleSchema tuple_schema_;
  CompositeTupleConditionFilter * condition_filter_ = nullptr;
  // TupleExtractor * tuple_extractor_;
};
#endif //__OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
