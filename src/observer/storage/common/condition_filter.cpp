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
#include "handler/handler_defs.h"
#include "record_manager.h"
#include "common/log/log.h"
#include "storage/common/table.h"

using namespace common;

ConditionFilter::~ConditionFilter() {
}

DefaultConditionFilter::DefaultConditionFilter() {
}
DefaultConditionFilter::~DefaultConditionFilter() {
}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right, AttrType attr_type, CompOp comp_op) {
  if (attr_type < chars || attr_type > floats) {
    LOG_ERROR("Invalid condition with unsupported attribute type: %d", attr_type);
    return RC::INVALID_ARGUMENT;
  }

  if (comp_op < EQual || comp_op >= NO_OP) {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  attr_type_ = attr_type;
  comp_op_ = comp_op;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, const Condition &condition) {
  const TableMeta &table_meta = table.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = undefined;
  AttrType type_right = undefined;

  if (1 == condition.bLhsIsAttr) {
    left.is_attr = true;
    const FieldMeta *field_left = table_meta.field(condition.lhsAttr.attrName);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.lhsAttr.attrName);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();

    type_left = field_left->type();
  } else {
    left.is_attr = false;
    left.value = condition.lhsValue.data; // 校验type 或者转换类型
    type_left = condition.lhsValue.type;
  }

  if (1 == condition.bRhsIsAttr) {
    right.is_attr = true;
    const FieldMeta *field_right = table_meta.field(condition.lhsAttr.attrName);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.lhsAttr.attrName);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();
  } else {
    right.is_attr = false;
    right.value = condition.rhsValue.data;
    type_right = condition.rhsValue.type;
  }

  // 校验和转换
//  if (!field_type_compare_compatible_table[type_left][type_right]) {
//    // TODO 不能比较的两个字段， 要把信息传给客户端
//    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
//  }
  if (type_left != type_right) {
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  return init(left, right, type_left, condition.op);
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
  switch (attr_type_) {
    case chars: { // 字符串都是定长的，直接比较
      // TODO 字符串的格式还不确定，先按照C字符串风格来定
      cmp_result = strcmp(left_value, right_value);
    }
    break;
    case ints: {
      // TODO 没有考虑大小端问题
      // TODO 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
      int left = *(int*)left_value;
      int right = *(int*)right_value;
      cmp_result = left - right;
    }
    break;
    case floats: {
      float left = *(float*)left_value;
      float right = *(float*)right_value;
      cmp_result = (int)(left - right);
    }
    break;
    default: {
      // TODO
    }
  }

  switch (comp_op_)
  {
  case EQual:
    return 0 == cmp_result;
  case LEqual:
    return cmp_result <= 0;
  case NEqual:
    return cmp_result != 0;
  case LessT:
    return cmp_result < 0;
  case GEqual:
    return cmp_result >= 0;
  case GreatT:
    return cmp_result > 0;
  
  default:
    break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result; // should not go here
}

CompositeConditionFilter::~CompositeConditionFilter() {
  if (memory_owner_) {
    delete filters_;
    filters_ = nullptr;
  }
}

RC CompositeConditionFilter::init(const ConditionFilter *filters, int filter_num, bool own_memory) {
  filters_ = filters;
  filter_num_ = filter_num;
  memory_owner_ = own_memory;
  return RC::SUCCESS;
}
RC CompositeConditionFilter::init(const ConditionFilter *filters, int filter_num) {
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
  DefaultConditionFilter *default_condition_filters = new DefaultConditionFilter();
  for (int i = 0; i < condition_num; i++) {
    rc = default_condition_filters[i].init(table, conditions[i]);
    if (rc != RC::SUCCESS) {
      delete default_condition_filters;
      default_condition_filters = nullptr;
      return rc;
    }
  }
  return init(default_condition_filters, condition_num, true);
}

bool CompositeConditionFilter::filter(const Record &rec) const {
  for (int i = 0; i < filter_num_; i++) {
    if (!filters_[i].filter(rec)) {
      return false;
    }
  }
  return true;
}

