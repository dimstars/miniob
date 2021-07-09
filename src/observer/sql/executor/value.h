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

#ifndef __OBSERVER_SQL_EXECUTOR_VALUE_H_
#define __OBSERVER_SQL_EXECUTOR_VALUE_H_

#include <string.h>

#include <string>
#include <ostream>

#include "sql/parser/parse.h"

class TupleValue {
public:
  TupleValue() = default;
  virtual ~TupleValue() = default;

  virtual void to_string(std::ostream &os) const = 0;
  virtual int compare(const TupleValue &other) const = 0;
  virtual void reset(TupleValue *other) = 0;
  virtual void set_type(AttrType type) { type_ = type;}
  virtual AttrType get_type() const { return type_;}

private:
  AttrType type_;   // 用来判断null // 为了做int/float的隐式转换 // TODO 优化
};

class IntValue : public TupleValue {
public:
  explicit IntValue(int value, AttrType type) : value_(value) {
    set_type(type);
  }

  void to_string(std::ostream &os) const override {
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const IntValue & int_other = (const IntValue &)other;
    return value_ - int_other.value_;
  }

  void reset(TupleValue *other) override{
    IntValue *int_other = (IntValue *)other;
    value_ = int_other->value_;
    delete other;
  }

  int get_value() const { return value_;}

private:
  int value_;
};

class FloatValue : public TupleValue {
public:
  explicit FloatValue(float value,  AttrType type) : value_(value) {
    set_type(type);
  }

  void to_string(std::ostream &os) const override {
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const FloatValue & float_other = (const FloatValue &)other;
    return (int)(value_ - float_other.value_);
  }

  void reset(TupleValue *other) override{
    FloatValue *int_other = (FloatValue *)other;
    value_ = int_other->value_;
    delete other;
  }

  float get_value() const { return value_;}

private:
  float value_;
};

class StringValue : public TupleValue {
public:
  StringValue(const char *value, int len, AttrType type) : value_(value, len){
    set_type(type);
  }
  explicit StringValue(const char *value, AttrType type) : value_(value) {
    set_type(type);
  }

  void to_string(std::ostream &os) const override {
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const StringValue &string_other = (const StringValue &)other;
    return strcmp(value_.c_str(), string_other.value_.c_str());
  }

  void reset(TupleValue *other) override{
    StringValue *int_other = (StringValue *)other;
    value_ = int_other->value_;
    delete other;
  }

private:
  std::string value_;
};

class DateValue : public TupleValue {
public: 
  explicit DateValue(unsigned int value, AttrType type) : value_(value) {
    set_type(type);
  }

  void to_string(std::ostream &os) const override {
    unsigned int year = value_ >> 11, month = (value_ & 0x7c0) >> 6, day = value_ & 0x3f;
    os << year << "-";
    month < 10 ? os << 0 << month << "-": os << month << "-";
    day < 10 ? os << 0 << day: os << day;
  }

  int compare(const TupleValue &other) const override {
    const DateValue & date_other = (const DateValue &)other;
    // TODO may not safe
    unsigned int value_r = date_other.value_;
    return value_ - value_r;
  }

  void reset(TupleValue *other) override{
    DateValue *int_other = (DateValue *)other;
    value_ = int_other->value_;
    delete other;
  }

private:
  unsigned int value_;
};

#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
