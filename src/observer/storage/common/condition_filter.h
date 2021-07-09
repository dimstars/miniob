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
// Created by Wangyunlai on 2021/5/7.
//

#ifndef __OBSERVER_STORAGE_COMMON_CONDITION_FILTER_H_
#define __OBSERVER_STORAGE_COMMON_CONDITION_FILTER_H_

#include <string>
#include "rc.h"
#include "sql/parser/parse.h"
#include "sql/executor/tuple.h"

struct Record;
class Table;

struct ConDesc {
  bool   is_attr;     // 是否属性，false 表示是值
  int    attr_length; // 如果是属性，表示属性值长度
  int    attr_offset; // 如果是属性，表示在记录中的偏移量
  void * value;       // 如果是值类型，这里记录值的数据
};

class ConditionFilter {
public:
  virtual ~ConditionFilter();

  virtual bool filter(const Record &rec) const = 0;
};

class DefaultConditionFilter : public ConditionFilter {
public:
  DefaultConditionFilter();
  virtual ~DefaultConditionFilter();

  RC init(const ConDesc &left, const ConDesc &right, AttrType type_left, AttrType type_right, CompOp comp_op);
  RC init(Table &table, const Condition &condition);

  virtual bool filter(const Record &rec) const;

public:
  const ConDesc &left() const {
    return left_;
  }

  const ConDesc &right() const {
    return right_;
  }

  const AttrType left_type() const {
    return type_left_;
  }

  const AttrType right_type() const {
    return type_right_;
  }

  CompOp comp_op() const {
    return comp_op_;
  }

private:
  ConDesc  left_;
  ConDesc  right_;
  AttrType type_left_;
  AttrType type_right_;
  CompOp   comp_op_;
};

class JoinConditionFilter : public ConditionFilter {
public:
  JoinConditionFilter();
  virtual ~JoinConditionFilter();

  RC init(const ConDesc &left, const ConDesc &right, AttrType type_left, AttrType type_right,
          const char * table_left, const char * table_right, const char * field_left, const char * field_right, CompOp comp_op);
  RC init(Table &table_left, Table &table_right, const Condition &condition);

  virtual bool filter(const Record &rec) const;
  bool filter(TupleSchema &schema, const Tuple &tuple) const;

public:
  const ConDesc &left() const {
    return left_;
  }

  const ConDesc &right() const {
    return right_;
  }

  CompOp comp_op() const {
    return comp_op_;
  }

private:
  ConDesc     left_;
  ConDesc     right_;
  AttrType    type_left_;
  AttrType    type_right_;
  std::string table_left_;
  std::string table_right_;
  std::string field_left_;
  std::string field_right_;
  CompOp      comp_op_;
};

class CompositeConditionFilter : public ConditionFilter {
public:
  CompositeConditionFilter() = default;
  virtual ~CompositeConditionFilter();

  RC init(const ConditionFilter *filters[], int filter_num);
  RC init(Table &table, const Condition *conditions, int condition_num);
  virtual bool filter(const Record &rec) const;

public:
  int filter_num() const {
    return filter_num_;
  }
  const ConditionFilter &filter(int index) const {
    return *filters_[index];
  }
  const ConditionFilter* filter() const {
    return *filters_;
  }

private:
  RC init(const ConditionFilter *filters[], int filter_num, bool own_memory);
private:
  const ConditionFilter **      filters_ = nullptr;
  int                           filter_num_ = 0;
  bool                          memory_owner_ = false; // filters_的内存是否由自己来控制
};

#endif // __OBSERVER_STORAGE_COMMON_CONDITION_FILTER_H_