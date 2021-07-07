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

#include "condition_filter.h"
#include "record_manager.h"
#include "common/log/log.h"
#include "storage/common/table.h"
#include "sql/executor/tuple.h"

using namespace common;

ConditionFilter::~ConditionFilter() {
}

DefaultConditionFilter::DefaultConditionFilter() {
}
DefaultConditionFilter::~DefaultConditionFilter() {
}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right, AttrType type_left, AttrType type_right, CompOp comp_op) {
  // 只需要判断一个，因为不相等的情况一定是int/float
  if (type_left < CHARS || type_left > DATES) {
    LOG_ERROR("Invalid condition with unsupported attribute type: %d", type_left);
    return RC::INVALID_ARGUMENT;
  }

  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  type_left_ = type_left;
  type_right_ = type_right;
  comp_op_ = comp_op;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, const Condition &condition) {
  const TableMeta &table_meta = table.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  if (1 == condition.left_is_attr) {
    left.is_attr = true;
    const FieldMeta *field_left = table_meta.field(condition.left_attr.attribute_name);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();

    type_left = field_left->type();
  } else {
    if(condition.left_value.type == DATES && !check_date_legality((char*)condition.left_value.data)) { // 校验date类型
      // TODO 没有考虑大小端问题
      LOG_WARN("Field value in condition is illeagl. %s.%s", table.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_VALUE_ILLEGAL;
    }
    left.is_attr = false;
    left.value = condition.left_value.data; // 校验type 或者转换类型
    type_left = condition.left_value.type;
  }

  if (1 == condition.right_is_attr) {
    right.is_attr = true;
    const FieldMeta *field_right = table_meta.field(condition.right_attr.attribute_name);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.right_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();
  } else {
    if(condition.right_value.type == DATES) { // 校验date类型
      // TODO 没有考虑大小端问题
      if(!check_date_legality((char*)condition.right_value.data)) {
        LOG_WARN("Field value in condition is illeagl. %s.%s", table.name(), condition.right_attr.attribute_name);
        return RC::SCHEMA_FIELD_VALUE_ILLEGAL;
      }
    }
    right.is_attr = false;
    right.value = condition.right_value.data;
    type_right = condition.right_value.type;
  }

  // 校验和转换
//  if (!field_type_compare_compatible_table[type_left][type_right]) {
//    // TODO 不能比较的两个字段， 要把信息传给客户端
//    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
//  }
  // NOTE：这里没有实现不同类型的数据比较，比如整数跟浮点数之间的对比
  // 但是选手们还是要实现。这个功能在预选赛中会出现
  if((type_left == FLOATS && type_right == INTS) || (type_left == INTS && type_right == FLOATS)) {
    // do nothing
    // 在filter里进行转换比较
  } else if(type_left != type_right) {
    LOG_WARN("Field type mismatch. left = %d, right = %d", type_left, type_right);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  return init(left, right, type_left, type_right, condition.comp);
}

bool DefaultConditionFilter::filter(const Record &rec) const {
  char *left_value = nullptr;
  char *right_value = nullptr;

  if (left_.is_attr) { // value
    left_value = (char *)(rec.data + left_.attr_offset);
  } else {
    left_value = (char *)left_.value;
  }

  if (right_.is_attr) {
    right_value = (char *)(rec.data + right_.attr_offset);
  } else {
    right_value = (char *)right_.value;
  }

  int cmp_result = 0;
  switch (type_left_) {
    case CHARS: { // 字符串都是定长的，直接比较
      // TODO 字符串的格式还不确定，先按照C字符串风格来定
      cmp_result = strcmp(left_value, right_value);
    }
    break;
    case INTS: {
      // TODO 没有考虑大小端问题
      // TODO 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
      if(type_right_ == INTS) {
        int left = *(int*)left_value;
        int right = *(int*)right_value;
        cmp_result = left - right;
      } else {
        int left = *(int*)left_value;
        float right = *(float*)right_value;
        cmp_result = (int)(left - right);
      }
    }
    break;
    case FLOATS: {
      if(type_right_ == FLOATS) {
        float left = *(float*)left_value;
        float right = *(float*)right_value;
        cmp_result = (int)(left - right);
      } else {
        float left = *(float*)left_value;
        int right = *(int*)right_value;
        cmp_result = (int)(left - right);
      }
    }
    break;
    case DATES: {
      cmp_result = cmp_date(left_value, right_value);
    }
    default: {
      // TODO
    }
  }

  switch (comp_op_)
  {
  case EQUAL_TO:
    return 0 == cmp_result;
  case LESS_EQUAL:
    return cmp_result <= 0;
  case NOT_EQUAL:
    return cmp_result != 0;
  case LESS_THAN:
    return cmp_result < 0;
  case GREAT_EQUAL:
    return cmp_result >= 0;
  case GREAT_THAN:
    return cmp_result > 0;
  
  default:
    break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result; // should not go here
}

JoinConditionFilter::JoinConditionFilter() {
}
JoinConditionFilter::~JoinConditionFilter() {
}

RC JoinConditionFilter::init(const ConDesc &left, const ConDesc &right, AttrType type_left, AttrType type_right,
                             const char * table_left, const char * table_right, const char * field_left, const char * field_right, CompOp comp_op) {
  // 只需要判断一个，因为不相等的情况一定是int/float
  if (type_left < CHARS || type_left > DATES) {
    LOG_ERROR("Invalid condition with unsupported attribute type: %d", type_left);
    return RC::INVALID_ARGUMENT;
  }

  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  type_left_ = type_left;
  type_right_ = type_right;
  table_left_ = std::string(table_left);
  table_right_ = std::string(table_right);
  field_left_ = std::string(field_left);
  field_right_ = std::string(field_right);
  comp_op_ = comp_op;
  return RC::SUCCESS;
}

RC JoinConditionFilter::init(Table &table_left, Table &table_right, const Condition &condition) {
  const TableMeta &table_meta_left = table_left.table_meta();
  const TableMeta &table_meta_right = table_right.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  if (1 == condition.left_is_attr) {
    left.is_attr = true;
    const FieldMeta *field_left = table_meta_left.field(condition.left_attr.attribute_name);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table_left.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();

    type_left = field_left->type();
  } else {
    if(condition.left_value.type == DATES && !check_date_legality((char*)condition.left_value.data)) { // 校验date类型
      // TODO 没有考虑大小端问题
      LOG_WARN("Field value in condition is illeagl. %s.%s", table_left.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_VALUE_ILLEGAL;
    }
    left.is_attr = false;
    left.value = condition.left_value.data; // 校验type 或者转换类型
    type_left = condition.left_value.type;
  }

  if (1 == condition.right_is_attr) {
    right.is_attr = true;
    const FieldMeta *field_right = table_meta_right.field(condition.right_attr.attribute_name);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table_right.name(), condition.right_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();
  } else {
    if(condition.right_value.type == DATES) { // 校验date类型
      // TODO 没有考虑大小端问题
      if(!check_date_legality((char*)condition.right_value.data)) {
        LOG_WARN("Field value in condition is illeagl. %s.%s", table_right.name(), condition.right_attr.attribute_name);
        return RC::SCHEMA_FIELD_VALUE_ILLEGAL;
      }
    }
    right.is_attr = false;
    right.value = condition.right_value.data;
    type_right = condition.right_value.type;
  }

  // 校验和转换
  if((type_left == FLOATS && type_right == INTS) || (type_left == INTS && type_right == FLOATS)) {
    // do nothing
    // 在filter里进行转换比较
  } else if(type_left != type_right) {
    LOG_WARN("Field type mismatch.");
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  const char * table_name_left  = table_left.name();
  const char * table_name_right = table_right.name();
  const char * field_name_left  = condition.left_attr.attribute_name;
  const char * field_name_right = condition.right_attr.attribute_name;

  return init(left, right, type_left, type_right, table_name_left, table_name_right, field_name_left, field_name_right, condition.comp);
}

bool JoinConditionFilter::filter(const Record &rec) const {
  return false;
}

TupleValue * value_to_tuple_value(const char *value, AttrType type) {
  switch(type) {
    case CHARS: {
      return new StringValue(value);
    }
    break;
    case INTS: {
      return new IntValue(*(int *)value);
    }
    break;
    case FLOATS: {
      return new FloatValue(*(float *)value);
    }
    break;
    case DATES: {
      return new DateValue(*(unsigned int *)value);
    }
    break;
    default: {
      return nullptr;
    }
  }
}

bool JoinConditionFilter::filter(TupleSchema &schema, const Tuple &tuple) const {
  char *left_value = nullptr;
  char *right_value = nullptr;
  TupleValue * left_tuple_value = nullptr;
  TupleValue * right_tuple_value = nullptr;

  if (left_.is_attr) { // value
    std::vector<const TupleField>::iterator field_iter = schema.fields().begin();
    std::vector<const std::shared_ptr<TupleValue> >::iterator value_iter = tuple.values().begin();
    for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
      if (0 == strcmp(field_iter->table_name(), table_left_.c_str()) && 0 == strcmp(field_iter->field_name(), field_left_.c_str())) {
        left_tuple_value = value_iter->get();
        break;
      }
    }
  } else {
    left_value = (char *)left_.value;
    left_tuple_value = value_to_tuple_value(left_value, type_left_);
  }

  if (right_.is_attr) {
    std::vector<const TupleField>::iterator field_iter = schema.fields().begin();
    std::vector<const std::shared_ptr<TupleValue> >::iterator value_iter = tuple.values().begin();
    for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
      if (0 == strcmp(field_iter->table_name(), table_right_.c_str()) && 0 == strcmp(field_iter->field_name(), field_right_.c_str())) {
        right_tuple_value = value_iter->get();
        break;
      }
    }
  } else {
    right_value = (char *)right_.value;
    right_tuple_value = value_to_tuple_value(right_value, type_right_);
  }

  if (left_tuple_value == nullptr || right_tuple_value == nullptr) {
    return false;
  }

  int cmp_result = left_tuple_value->compare(*right_tuple_value);

  switch (comp_op_)
  {
  case EQUAL_TO:
    return 0 == cmp_result;
  case LESS_EQUAL:
    return cmp_result <= 0;
  case NOT_EQUAL:
    return cmp_result != 0;
  case LESS_THAN:
    return cmp_result < 0;
  case GREAT_EQUAL:
    return cmp_result >= 0;
  case GREAT_THAN:
    return cmp_result > 0;
  
  default:
    break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result; // should not go here
}

CompositeConditionFilter::~CompositeConditionFilter() {
  if (memory_owner_) {
    delete[] filters_;
    filters_ = nullptr;
  }
}

RC CompositeConditionFilter::init(const ConditionFilter *filters[], int filter_num, bool own_memory) {
  filters_ = filters;
  filter_num_ = filter_num;
  memory_owner_ = own_memory;
  return RC::SUCCESS;
}
RC CompositeConditionFilter::init(const ConditionFilter *filters[], int filter_num) {
  return init(filters, filter_num, false);
}

RC CompositeConditionFilter::init(Table &table, const Condition *conditions, int condition_num) {
  if (condition_num == 0) {
    return RC::SUCCESS;
  }
  if (conditions == nullptr) {
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;
  ConditionFilter **condition_filters = new ConditionFilter *[condition_num];
  for (int i = 0; i < condition_num; i++) {
    DefaultConditionFilter *default_condition_filter = new DefaultConditionFilter();
    rc = default_condition_filter->init(table, conditions[i]);
    if (rc != RC::SUCCESS) {
      delete default_condition_filter;
      for (int j = i - 1; j >= 0; j--) {
        delete condition_filters[j];
        condition_filters[j] = nullptr;
      }
      delete[] condition_filters;
      condition_filters = nullptr;
      return rc;
    }
    condition_filters[i] = default_condition_filter;
  }
  return init((const ConditionFilter **)condition_filters, condition_num, true);
}

bool CompositeConditionFilter::filter(const Record &rec) const {
  for (int i = 0; i < filter_num_; i++) {
    if (!filters_[i]->filter(rec)) {
      return false;
    }
  }
  return true;
}

