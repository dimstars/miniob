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
#include "common/lang/bitmap.h"
#include "storage/common/table.h"
#include "sql/executor/tuple.h"

using namespace common;

ConditionFilter::~ConditionFilter() {
}

DefaultConditionFilter::DefaultConditionFilter(Table &table): table_(table) {
}
DefaultConditionFilter::~DefaultConditionFilter() {
  //TODO 释放ConDesc里的value.data空间
}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right, AttrType type_left, AttrType type_right, CompOp comp_op, TupleSet *tuple_set) {
  if (type_left < CHARS || type_left > NULLS) {
    LOG_ERROR("Invalid condition with unsupported left attribute type: %d", type_left);
    return RC::INVALID_ARGUMENT;
  }
  if (type_right < CHARS || type_right > NULLS) {
    LOG_ERROR("Invalid condition with unsupported right attribute type: %d", type_right);
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
  tuple_set_ = tuple_set;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, const Condition &condition, TupleSet *tuple_set) {
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
    left.attr_index = field_left->index();
    left.nullable = field_left->nullable();

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
    right.attr_index = field_right->index();
    right.nullable = field_right->nullable();

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

  // TODO 优化过滤
  // if(left.is_attr && !left.nullable && condition.comp == EQUAL_TO && !right.is_attr && type_right == NULLS) {
  //   LOG_WARN("Field left is not nullable. so filter all");
  //   return RC::SCHEMA_CONDITION_FILTER_ALL;
  // } 
  LOG_ERROR("left type %d, right type %d, op %d", type_left, type_right, condition.comp);
  if (condition.sub_selects != nullptr) {

  } else if(left.is_attr && !left.nullable && condition.comp == NOT_EQUAL && !right.is_attr && type_right == NULLS) {
    LOG_ERROR("Field left is not nullable. so filter nothing");
    return RC::SCHEMA_CONDITION_INVALID;
  } else if((type_left == FLOATS && type_right == INTS) 
    || (type_left == INTS && type_right == FLOATS)
    || type_right == NULLS || type_left == NULLS
    || (type_left == TEXTS && type_right == CHARS)
    || (type_left == CHARS && type_right == TEXTS)) {
    // do nothing
    // 在filter里进行转换比较
    LOG_WARN("Field type mismatch. left = %d, right = %d", type_left, type_right);
  } else if(type_left != type_right) {
    LOG_WARN("Field type mismatch. left = %d, right = %d", type_left, type_right);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  return init(left, right, type_left, type_right, condition.comp, tuple_set);
}

bool DefaultConditionFilter::filter(const Record &rec) const {
  char *left_value = nullptr;
  char *right_value = nullptr;

  Bitmap bitmap(rec.data, RECORD_BITMAP_BITS);

  // 如果是子查询条件
  if (comp_op_ == WHERE_IN) {
    if (tuple_set_->size() == 0) {
      return false;
    }
    left_value = (char *)(rec.data + left_.attr_offset);
    TupleValue *left_tuple_value;
    switch (type_left_) {
      case CHARS:
        left_tuple_value = new StringValue(left_value, type_left_);
        break;
      case INTS:
        left_tuple_value = new IntValue(*(int*)left_value, type_left_);
        break;
      case FLOATS:
        left_tuple_value = new FloatValue(*(float*)left_value, type_left_);
        break;
      case DATES:
        left_tuple_value = new DateValue(*(unsigned int*)left_value, type_left_);
        break;
      case NULLS:
        left_tuple_value = new StringValue(left_value, type_left_);
        break;
      default:
        return false;
    }
    for (int i = 0; i < tuple_set_->size(); i++) {
      const TupleValue &tuple_value = tuple_set_->tuples()[i].get(0);
      if (type_left_ == tuple_value.get_type() && left_tuple_value->compare(tuple_value) == 0) {
        return true;
      }
    }
    return false;
  }

  // 首先处理任何出现null的情况
  if (left_.is_attr && !right_.is_attr && type_right_ == NULLS) { // WARN 一般是不会出现左右都是属性的情况
    // id is / is not null or id op null
    LOG_ERROR("comp op = %d", comp_op_);
    switch(comp_op_) {
      case IS_NULL: {
        return bitmap.get_bit(left_.attr_index);
      }
      break;
      case IS_NOT_NULL: {
        return !bitmap.get_bit(left_.attr_index);
      }
      break;
      case NO_OP: {
        LOG_PANIC("Never should print this.");
      }
      break;
      default:
        return false;
    }
  } 
  if (left_.is_attr && bitmap.get_bit(left_.attr_index) && !right_.is_attr && type_right_ != NULLS) {
    // 左边是属性，并且取出来的record是null，而右边不是null
    // id op value
    return false;
  }

  if (!left_.is_attr && type_left_ == NULLS) {
    if(type_right_ == NULLS && comp_op_ == IS_NULL) return true; // null is null
    else return false; // null is not null or null op value or null op id
  }

  if(right_.is_attr && bitmap.get_bit(right_.attr_index)) return false; // value op id 

  if(!right_.is_attr && type_right_ == NULLS) {
    if(comp_op_ == IS_NOT_NULL) return true; // value is not null
    else return false;  // value is null or value op null
  }

  // 以下左右都不为null
  if (left_.is_attr) { // value
    if(type_left_ == TEXTS) {
      OID oid = *(OID*)(rec.data + left_.attr_offset);
      char *data = new char [oid.len];
      table_.get_record(&oid, data);
      left_value = data;
    } else {
      left_value = (char *)(rec.data + left_.attr_offset);
    }
  } else {
    left_value = (char *)left_.value;
  }

  if (right_.is_attr) {
    if(type_right_ == TEXTS) {
      OID oid = *(OID*)(rec.data + right_.attr_offset);
      char *data = new char [oid.len];
      table_.get_record(&oid, data);
      right_value = data;
    } else {
      right_value = (char *)(rec.data + right_.attr_offset);
    }
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
        // eg. 1 op 1.5
        float left = (float)(*(int*)left_value);
        float right = *(float*)right_value;
        switch (comp_op_)
        {
        case EQUAL_TO:
          return left == right;
        case LESS_EQUAL:
          return left <= right;
        case NOT_EQUAL:
          return left != right;
        case LESS_THAN:
          return left < right;
        case GREAT_EQUAL:
          return left >= right;
        case GREAT_THAN:
          return left > right;
        default:
          break;
        }
      }
    }
    break;
    case FLOATS: {
      float left = *(float*)left_value;
      float right = type_right_ == FLOATS ? *(float*)right_value : (float)(*(int*)right_value);
      switch (comp_op_)
      {
      case EQUAL_TO:
        return left == right;
      case LESS_EQUAL:
        return left <= right;
      case NOT_EQUAL:
        return left != right;
      case LESS_THAN:
        return left < right;
      case GREAT_EQUAL:
        return left >= right;
      case GREAT_THAN:
        return left > right;
      default:
        break;
      }
    }
    break;
    case DATES: {
      cmp_result = cmp_date(left_value, right_value);
    }
    break;
    case TEXTS: {
      cmp_result = strcmp(left_value, right_value);
      if (left_.is_attr && type_left_ == TEXTS) delete left_value;
      if (right_.is_attr && type_right_ == TEXTS) delete right_value;
    }
    break;
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
  if(condition.left_exp){
    left_exp_ = condition.left_exp;
  }else{
    left.is_attr = true;
    const FieldMeta *field_left = table_meta_left.field(condition.left_attr.attribute_name);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table_left.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();
    type_left = field_left->type();
  }
  if(condition.right_exp){
    right_exp_ = condition.right_exp;
  }else{
    right.is_attr = true;
    const FieldMeta *field_right = table_meta_right.field(condition.right_attr.attribute_name);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table_right.name(), condition.right_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();
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

bool JoinConditionFilter::filter(TupleSchema &schema, const Tuple &tuple) const {
  double left,right;
  std::vector<const TupleField>::iterator field_iter;
  std::vector<const std::shared_ptr<TupleValue> >::iterator value_iter;
  TupleValue *left_date = nullptr;
  TupleValue *right_date = nullptr;
  if(left_exp_){
    left = CalculateExp(left_exp_, schema, tuple);
  }else{
    field_iter = schema.fields().begin();
    value_iter = tuple.values().begin();
    TupleValue * tuple_value = nullptr;
    for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
      if (0 == strcmp(field_iter->table_name(), table_left_.c_str()) && 0 == strcmp(field_iter->field_name(), field_left_.c_str())) {
        tuple_value = value_iter->get();
        break;
      }
    }
    if(tuple_value == nullptr) return false;
    // 任何null值不满足任何比较条件，只有is null或is not null支持null比较
    if(tuple_value->get_type() == NULLS ) return false;
    if(tuple_value->get_type() == INTS){
      IntValue *tuple_tmp_value = dynamic_cast<IntValue *>(tuple_value);
      left = tuple_tmp_value->get_value();
    }else if(tuple_value->get_type() == FLOATS){
      FloatValue *tuple_tmp_value = dynamic_cast<FloatValue *>(tuple_value);
      left = tuple_tmp_value->get_value();
    }else{//DATES
      left_date = tuple_value;
    }
  }

  if(right_exp_){
    right = CalculateExp(right_exp_, schema, tuple);
  }else{
    field_iter = schema.fields().begin();
    value_iter = tuple.values().begin();
    TupleValue * tuple_value = nullptr;
    for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
      if (0 == strcmp(field_iter->table_name(), table_right_.c_str()) && 0 == strcmp(field_iter->field_name(), field_right_.c_str())) {
        tuple_value = value_iter->get();
        break;
      }
    }
    if(tuple_value == nullptr) return false;
    // 任何null值不满足任何比较条件，只有is null或is not null支持null比较
    if(tuple_value->get_type() == NULLS ) return false;
    if(tuple_value->get_type() == INTS){
      IntValue *tuple_tmp_value = dynamic_cast<IntValue *>(tuple_value);
      right = tuple_tmp_value->get_value();
    }else if(tuple_value->get_type() == FLOATS){
      FloatValue *tuple_tmp_value = dynamic_cast<FloatValue *>(tuple_value);
      right = tuple_tmp_value->get_value();
    }else{//DATES
      right_date = tuple_value;
    }
  }

  int cmp_result = 0;
  if(right_date && left_date)
    cmp_result = left_date->compare(*right_date);
  else{
    cmp_result = left - right;
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

double JoinConditionFilter::CalculateExp(const CalExp *exp, const TupleSchema &schema, const Tuple &tuple) const {
  double res = 0;
  if(!exp)
    return res;
  if(exp->left_exp == nullptr && exp->right_exp == nullptr){
    //value
    if(exp->is_attr){
      std::vector<const TupleField>::iterator field_iter = schema.fields().begin();
      std::vector<const std::shared_ptr<TupleValue> >::iterator value_iter = tuple.values().begin();
      for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
        if (0 == strcmp(field_iter->table_name(), exp->attr.relation_name) && 0 == strcmp(field_iter->field_name(), exp->attr.attribute_name)) {
          TupleValue * tuple_value = value_iter->get();
          if(tuple_value->get_type() == INTS){
            IntValue *tmp_value = dynamic_cast<IntValue *>(tuple_value);
            res = tmp_value->get_value();
          }else if(tuple_value->get_type() == FLOATS){
            FloatValue *tmp_value = dynamic_cast<FloatValue *>(tuple_value);
            res = tmp_value->get_value();
          }else{
            //格式错误
          }
          break;
        }
      }
    }else{
      if(exp->value.type == INTS){
        res = (double)*(int*)exp->value.data;
      }else if(exp->value.type == FLOATS){
        res = (double)*(float*)exp->value.data;
      }
    }
  }else if(exp->left_exp != nullptr && exp->right_exp != nullptr){
    //exp
    double left = CalculateExp(exp->left_exp,schema,tuple);
    double right = CalculateExp(exp->right_exp,schema,tuple);
    switch (exp->com_op){
      case ADD:
        res = left + right;
        break;
      case SUB:
        res = left - right;
        break;
      case MUL:
        res = left * right;
        break;
      case DIV:
        res = left / right;
        break;
      default:
        //格式错误
        break;
    }
  }else{
    //格式错误
  }
  return res;
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
    DefaultConditionFilter *default_condition_filter = new DefaultConditionFilter(table);
    rc = default_condition_filter->init(table, conditions[i], NULL);
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

