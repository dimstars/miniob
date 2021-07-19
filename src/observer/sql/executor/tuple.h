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
// Created by Wangyunlai on 2021/5/14.
//

#ifndef __OBSERVER_SQL_EXECUTOR_TUPLE_H_
#define __OBSERVER_SQL_EXECUTOR_TUPLE_H_

#include <memory>
#include <vector>

#include "sql/parser/parse.h"
#include "sql/executor/value.h"

class Table;
struct OID;

// 为了进行聚合运算所维护的统计变量
typedef struct _MaxStat {
  Value value;
} MaxStat;

typedef struct _MinStat {
  Value value;
} MinStat;

typedef struct _CountStat {
  int count;
} CountStat;

typedef struct _AvgStat {
  int count;
  float sum;
} AvgStat;

union TupleStats {
  MaxStat maxs;
  MinStat mins;
  CountStat counts;
  AvgStat avgs;
};

// 目前只在TupleField使用，因此TupleField实现管理
typedef struct TupleStat {
  union TupleStats stat;
} TupleStat;

class Tuple {
public:
  Tuple() = default;

  Tuple(const Tuple &other);

  ~Tuple();

  Tuple(Tuple &&other) noexcept ;
  Tuple & operator=(Tuple &&other) noexcept ;

  void add(TupleValue *value);
  void add(const std::shared_ptr<TupleValue> &other);
  void add(int value, AttrType type);
  void add(unsigned int value, AttrType type);
  void add(float value, AttrType type);
  void add(const char *s, int len, AttrType type);

  void reset(int value, int index);
  void reset(unsigned int value, int index);
  void reset(float value, int index);
  void reset(const char *s, int len, int index);

  const std::vector<std::shared_ptr<TupleValue>> &values() const {
    return values_;
  }

  int size() const {
    return values_.size();
  }

  const TupleValue &get(int index) const {
    return *values_[index];
  }

  const std::shared_ptr<TupleValue> &get_pointer(int index) const {
    return values_[index];
  }

private:
  std::vector<std::shared_ptr<TupleValue>>  values_;
};

// 为了支持聚合运算，存放聚合运算关系
class TupleField {
public:
  TupleField(const TupleField &other) :
          type_(other.type()), atype_(other.atype()), table_name_(other.table_name()), field_name_(other.field_name()) {
    stat_reset(other.stat());
  }
  TupleField(AttrType type, AggType atype, const char *table_name, const char *field_name) :
          type_(type), atype_(atype), table_name_(table_name), field_name_(field_name){
    stat_create();
  }

  // TupleStat functions
  void stat_create();
  void stat_init();
  void stat_destroy();
  void stat_value_destroy();
  void stat_reset(const TupleStat *other);
  void stat_value_reset(Value *value, const Value *other);

  TupleStat *stat() const {
    return stat_;
  }

  AttrType  type() const{
    return type_;
  }
  
  AggType atype() const {
    return atype_;
  }

  const char *table_name() const {
    return table_name_.c_str();
  }
  const char *field_name() const {
    return field_name_.c_str();
  }

  std::string to_string() const;
private:
  AttrType  type_;
  AggType     atype_;
  TupleStat  *stat_;
  std::string table_name_;
  std::string field_name_;
};

// 用来存对于某表需要返回哪些列（TupleField，表名.列名）
class TupleSchema {
public:
  TupleSchema() = default;
  ~TupleSchema() = default;

  void add(AttrType type, AggType atype, const char *table_name, const char *field_name);
  void add_if_not_exists(AttrType type, AggType atype, const char *table_name, const char *field_name);
  // void merge(const TupleSchema &other);
  void append(const TupleSchema &other);

  const std::vector<TupleField> &fields() const {
    return fields_;
  }

  const TupleField &field(int index) const {
    return fields_[index];
  }

  int index_of_field(AggType atype, const char *table_name, const char *field_name) const;

  void clear() {
    for(int i = 0; i < fields_.size(); i++) {
      fields_[i].stat_destroy();
    }
    fields_.clear();
  }

  int size() {
    return fields_.size();
  }

  void print(std::ostream &os) const;
public:
  static void from_table(const Table *table, TupleSchema &schema);
private:
  std::vector<TupleField> fields_;
};

class TupleSet {
public:
  TupleSet() = default;
  TupleSet(TupleSet &&other);
  explicit TupleSet(const TupleSchema &schema) : schema_(schema) {
  }
  TupleSet &operator =(TupleSet &&other);

  ~TupleSet() = default;

  void set_schema(const TupleSchema &schema);

  const TupleSchema &get_schema() const;

  void add(Tuple && tuple);

  void clear();

  void clear_tuples();

  void reset(int value, int index) {
    for(int i = 0; i < size(); i++) {
      tuples_[i].reset(value, index);
    }
  }

  void reset(unsigned int value, int index) {
    for(int i = 0; i < size(); i++) {
      tuples_[i].reset(value, index);
    }
  }

  void reset(float value, int index) {
    for(int i = 0; i < size(); i++) {
      tuples_[i].reset(value, index);
    }
  }

  void reset(const char *s, int len, int index) {
    for(int i = 0; i < size(); i++) {
      tuples_[i].reset(s, len, index);
    }
  }

  bool is_empty() const;
  int size() const;
  const Tuple &get(int index) const;
  const std::vector<Tuple> &tuples() const;

  void print(std::ostream &os) const;
  void print(CalExp * const *exprs, int num, bool print_relation, std::ostream &os);
public:
  const TupleSchema &schema() const {
    return schema_;
  }
private:
  std::vector<Tuple> tuples_;
  TupleSchema schema_;
};

class TupleRecordConverter {
public:
  TupleRecordConverter(Table *table, TupleSet &tuple_set);

  void add_record(const char *record);
private:
  Table *table_;
  TupleSet &tuple_set_;
};

#endif //__OBSERVER_SQL_EXECUTOR_TUPLE_H_
