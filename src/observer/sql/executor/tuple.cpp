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

#include "sql/executor/tuple.h"
#include "storage/common/record_manager.h"
#include "storage/common/table.h"
#include "common/log/log.h"
#include "common/lang/bitmap.h"

using namespace common;

Tuple::Tuple(const Tuple &other) {
  LOG_PANIC("Copy constructor of tuple is not supported");
  exit(1);
}

Tuple::Tuple(Tuple &&other) noexcept : values_(std::move(other.values_)) {
}

Tuple & Tuple::operator=(Tuple &&other) noexcept {
  if (&other == this) {
    return *this;
  }

  values_.clear();
  values_.swap(other.values_);
  return *this;
}

Tuple::~Tuple() {
}

// TODO add (Value && value)
void Tuple::add(TupleValue *value) {
  values_.emplace_back(value);
}
void Tuple::add(const std::shared_ptr<TupleValue> &other) {
  values_.emplace_back(other);
}
void Tuple::add(int value, AttrType type) {
  add(new IntValue(value, type));
}

void Tuple::add(unsigned int value, AttrType type) {
  add(new DateValue(value, type));
}

void Tuple::add(float value, AttrType type) {
  add(new FloatValue(value, type));
}

void Tuple::add(const char *s, int len, AttrType type) {
  add(new StringValue(s, len, type));
}

void Tuple::reset(int value, int index) {
  const std::shared_ptr<TupleValue> &p = get_pointer(index);
  (*p).reset(new IntValue(value, INTS));
}

void Tuple::reset(unsigned int value, int index) {
  const std::shared_ptr<TupleValue> &p = get_pointer(index);
  (*p).reset(new DateValue(value, DATES));
}

void Tuple::reset(float value, int index) {
  const std::shared_ptr<TupleValue> &p = get_pointer(index);
  (*p).reset(new FloatValue(value, FLOATS));
}

void Tuple::reset(const char *s, int len, int index) {
  const std::shared_ptr<TupleValue> &p = get_pointer(index);
  (*p).reset(new StringValue(s, len, CHARS));
}
////////////////////////////////////////////////////////////////////////////////
static std::string agg_type_to_string(AggType atype) {
  switch (atype) {
    case MAX_A:
      return "max";
      break;
    case MIN_A:
      return "min";
      break;
    case COUNT_A:
      return "count";
      break;
    case AVG_A:
      return "avg";
      break;
    default:
      return "";
      break;
  }
}

static void stat_value_init(Value *value, AttrType type) {
  switch (type) {
    case CHARS: {
      value_init_string(value, "");
    }
    break;
    case INTS: {
      value_init_integer(value, 0);
    } 
    break;
    case FLOATS: {
      value_init_float(value, 0.0);
    }
    break;
    case DATES: {
      value_init_date(value, "2000-01-01");
    }
    case NULLS: {
      
    }
    break;
    default:
    break;
  }
}

// 该函数只在TupleField构造函数调用
void TupleField::stat_create() {
  if(atype_ <= AVG_A) {
      stat_ = (TupleStat *)malloc(sizeof(TupleStat));
      if(nullptr == stat_) {
        LOG_ERROR("Failed to alloc memroy for TupleStat. size=%d", sizeof(TupleStat));
        // TODO 异常处理
        return;      
      }
      stat_init();
    } else {
      stat_ = nullptr;
    }
}

void TupleField::stat_init() {
  switch (atype_) {
    case MAX_A: {
      memset(&stat_->stat, 0, sizeof(stat_->stat));
      stat_value_init(&stat_->stat.maxs.value, type_);
    }
    break;
    case MIN_A: {
      memset(&stat_->stat, 0, sizeof(stat_->stat));
      stat_value_init(&stat_->stat.mins.value, type_);
    }
    break;
    case COUNT_A: {
      memset(&stat_->stat, 0, sizeof(stat_->stat));
    }
    break;
    case AVG_A: {
      memset(&stat_->stat, 0, sizeof(stat_->stat));
    }
    break;
    default:
    // 不会走到这个分支    
    break;
  }
}

void TupleField::stat_destroy() {
  if(stat_ != nullptr) {
    stat_value_destroy();
    free(stat_);
    stat_ = nullptr;
  }
}

void TupleField::stat_value_destroy() {
  switch (atype_) {
    case MAX_A: {
      value_destroy(&stat_->stat.maxs.value);
    }
    break;
    case MIN_A: {
      value_destroy(&stat_->stat.mins.value);
    }
    break;
    case COUNT_A:
    case AVG_A:
    case NOTHING_A:
    break;
  }
}

void TupleField::stat_reset(const TupleStat *other) {
  stat_ = (TupleStat *)malloc(sizeof(TupleStat));
  if(nullptr == stat_) {
    LOG_ERROR("Failed to alloc memroy for TupleStat. size=%d", sizeof(TupleStat));
    // TODO 异常处理
    return;      
  }
  switch (atype_) {
    case MAX_A: {
      stat_value_reset(&stat_->stat.maxs.value, &other->stat.maxs.value);
    }
    break;
    case MIN_A: {
      stat_value_reset(&stat_->stat.mins.value, &other->stat.mins.value); 
    }
    break;
    case COUNT_A: {
      stat_->stat.counts.count = other->stat.counts.count;
    }
    break;
    case AVG_A: {
      stat_->stat.avgs.count = other->stat.avgs.count;
      stat_->stat.avgs.sum = other->stat.avgs.sum;
    }
    break;
    case NOTHING_A:
    break;
  }
}

void TupleField::stat_value_reset(Value *value, const Value *other) {
  switch (other->type) {
    case CHARS: {
      value_init_string(value, (char*)other->data);
    }
    break;
    case INTS: {
      value_init_integer(value, *(int*)other->data);
    } 
    break;
    case FLOATS: {
      value_init_float(value, *(float*)other->data);
    }
    break;
    case DATES: {
      value->type = DATES;
      unsigned int data = *(unsigned int*)other->data;
      value->data = malloc(sizeof(data));
      memcpy(value->data, &data, sizeof(data));
    }
    break;
    default:
    break;
  }
}
std::string TupleField::to_string() const {
  // TODO 修改tostring
  return std::string(table_name_) + "." + field_name_ + std::to_string(type_);
}

////////////////////////////////////////////////////////////////////////////////
void TupleSchema::from_table(const Table *table, TupleSchema &schema) {
  const char *table_name = table->name();
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = 0; i < field_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i);
    if (field_meta->visible()) {
      schema.add(field_meta->type(), NOTHING_A, table_name, field_meta->name());
    }
  }
}

void TupleSchema::add(AttrType type, AggType atype, const char *table_name, const char *field_name) {
  fields_.emplace_back(type, atype, table_name, field_name);
}

void TupleSchema::add_if_not_exists(AttrType type, AggType atype, const char *table_name, const char *field_name) {
  for (const auto &field: fields_) {
    if (atype == field.atype() &&
        0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name)) {
      return;
    }
  }

  add(type, atype, table_name, field_name);
}

void TupleSchema::append(const TupleSchema &other) {
  fields_.reserve(fields_.size() + other.fields_.size());
  for (const auto &field: other.fields_) {
    fields_.emplace_back(field);
  }
}

int TupleSchema::index_of_field(AggType atype, const char *table_name, const char *field_name) const {
  const int size = fields_.size();
  for (int i = 0; i < size; i++) {
    const TupleField &field = fields_[i];
    if (atype == field.atype() && 
        0 == strcmp(field.table_name(), table_name) && 0 == strcmp(field.field_name(), field_name)) {
      return i;
    }
  }
  return -1;
}

void TupleSchema::print(std::ostream &os) const {
  if (fields_.empty()) {
    os << "No schema";
    return;
  }

  // 判断有多张表还是只有一张表
  std::set<std::string> table_names;
  for (const auto &field: fields_) {
    table_names.insert(field.table_name());
  }

  for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end = --fields_.end();
       iter != end; ++iter) {
    if(iter->atype() == NOTHING_A) {
      if (table_names.size() > 1 && iter->table_name() != nullptr) {
        os << iter->table_name() << ".";
      }
      os << iter->field_name() << " | ";
    } else {
      os << agg_type_to_string(iter->atype()) << "(";
      if(table_names.size() > 1 && iter->table_name() != nullptr) {
        os << iter->table_name() << ".";
      }
      os << iter->field_name() << ")" << " | ";
    }
  }

  if(fields_.back().atype() == NOTHING_A) {
    if (table_names.size() > 1 && fields_.back().table_name() != nullptr) {
      os << fields_.back().table_name() << ".";
    }
    os << fields_.back().field_name() << std::endl;
  } else {
    os << agg_type_to_string(fields_.back().atype()) << "(";
    if(table_names.size() > 1 && fields_.back().table_name() != nullptr) {
      os << fields_.back().table_name() << ".";
    }
    os << fields_.back().field_name() << ")" << std::endl;
  }
}

/////////////////////////////////////////////////////////////////////////////
TupleSet::TupleSet(TupleSet &&other) : tuples_(std::move(other.tuples_)), schema_(other.schema_){
  other.schema_.clear();
}

TupleSet &TupleSet::operator=(TupleSet &&other) {
  if (this == &other) {
    return *this;
  }

  schema_.clear();
  schema_.append(other.schema_);
  other.schema_.clear();

  tuples_.clear();
  tuples_.swap(other.tuples_);
  return *this;
}

void TupleSet::add(Tuple &&tuple) {
  tuples_.emplace_back(std::move(tuple));
}

void TupleSet::clear() {
  tuples_.clear();
  schema_.clear();
}

void TupleSet::print(std::ostream &os) const {
  if (schema_.fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  schema_.print(os);

  for (const Tuple &item : tuples_) {
    const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
    for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
          iter != end; ++iter) {
      (*iter)->to_string(os);
      os << " | ";
    }
    values.back()->to_string(os);
    os << std::endl;
  }
}

void TupleSet::set_schema(const TupleSchema &schema) {
  schema_ = schema;
}

const TupleSchema &TupleSet::get_schema() const {
  return schema_;
}

bool TupleSet::is_empty() const {
  return tuples_.empty();
}

int TupleSet::size() const {
  return tuples_.size();
}

const Tuple &TupleSet::get(int index) const {
  return tuples_[index];
}

const std::vector<Tuple> &TupleSet::tuples() const {
  return tuples_;
}

/////////////////////////////////////////////////////////////////////////////
// 非聚合运算
static bool normal_tuple_add(const FieldMeta *field_meta, Tuple &tuple, const char *record, Table *table) {
  Bitmap bitmap(const_cast<char*>(record), RECORD_BITMAP_BITS);
  // assert(field_meta != nullptr);
  switch (field_meta->type()) {
    case INTS: {
      if(bitmap.get_bit(field_meta->index())) {
        const char *s = "null";
        tuple.add(s, strlen(s), NULLS);
      } else {
        int value = *(int*)(record + field_meta->offset());
        tuple.add(value, INTS);
      }
    }
    break;
    case FLOATS: {
      if(bitmap.get_bit(field_meta->index())) {
        const char *s = "null";
        tuple.add(s, strlen(s), NULLS);
      } else {
        float value = *(float *)(record + field_meta->offset());
        tuple.add(value, FLOATS);
      }
    }
      break;
    case CHARS: {
      if(bitmap.get_bit(field_meta->index())) {
        const char *s = "null";
        tuple.add(s, strlen(s), NULLS);
      } else {
        const char *s = record + field_meta->offset();  // 现在当做Cstring来处理
        tuple.add(s, strlen(s), CHARS);
      }
    }
    break;
    case DATES: {
      if(bitmap.get_bit(field_meta->index())) {
        const char *s = "null";
        tuple.add(s, strlen(s), NULLS);
      } else {
        unsigned int value = *(unsigned int*)(record + field_meta->offset()); 
        tuple.add(value, DATES);
      }
    }
    break;
    case TEXTS: {
      if(bitmap.get_bit(field_meta->index())) {
        const char *s = "null";
        tuple.add(s, strlen(s), NULLS);
      } else {
        OID oid = *(OID*)(record + field_meta->offset());
        char *s = new char [oid.len];
        table->get_record(&oid, s);
        tuple.add(s, oid.len, CHARS);
      }
    }
    break;
    default: {
      LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
    }
  }
  return true;
}

/** TODO
 * 聚合运算，目前只考虑只有聚合运算或只有非聚合运算的情况，不考虑group by的情况
 * 这里实现为每条record计算聚合运算结果，更新TupleSet中的Tuple
 * 也可以选择实现为每条record都加入TupleSet并计算聚合运算结果，但不更新Tuple，最后清空TupleSet，插入一条聚合运算结果
 */ 
// MAX运算
static bool max_tuple_add(TupleSet &tuple_set, const TupleField &field, \
  const FieldMeta *field_meta, Tuple &tuple, const char *record, int index) {
  // assert(field_meta != nullptr);
  assert(field.stat() != nullptr);
  Bitmap bitmap(const_cast<char*>(record), RECORD_BITMAP_BITS);
  if(bitmap.get_bit(field_meta->index())) return false;

  switch (field_meta->type()) {
    case INTS: {
      int value = *(int*)(record + field_meta->offset());
      if(tuple_set.is_empty()) {
        *(int*)(field.stat()->stat.maxs.value.data) = value;
        tuple.add(value, INTS);
        return true;
      } else if(value > *(int*)(field.stat()->stat.maxs.value.data)) {
        *(int*)(field.stat()->stat.maxs.value.data) = value;
        tuple_set.reset(value, index);
      }
    }
    break;
    case FLOATS: {
      float value = *(float *)(record + field_meta->offset());
      if(tuple_set.is_empty()) {
        *(float*)(field.stat()->stat.maxs.value.data) = value;
        tuple.add(value, FLOATS);
        return true;
      } else if(value > *(float*)(field.stat()->stat.maxs.value.data)) {
        *(float*)(field.stat()->stat.maxs.value.data) = value;
        tuple_set.reset(value, index);
      }
    }
    break;
    case CHARS: {
      const char *s = record + field_meta->offset();  // 现在当做Cstring来处理
      if(tuple_set.is_empty()) {
        free(field.stat()->stat.maxs.value.data);
        value_init_string(&(field.stat()->stat.maxs.value), s);
        tuple.add(s, strlen(s), CHARS);
        return true;
      } else if(0 < strcmp(s, (char*)(field.stat()->stat.maxs.value.data))) {
          free(field.stat()->stat.maxs.value.data);
          value_init_string(&(field.stat()->stat.maxs.value), s);
          tuple_set.reset(s, strlen(s), index);
      }
    }
    break;
    case DATES: {
      unsigned int value = *(unsigned int*)(record + field_meta->offset()); 
      if(tuple_set.is_empty()) {
        *(unsigned int*)(field.stat()->stat.maxs.value.data) = value;
        tuple.add(value, DATES);
        return true;
      } else if(value > *(unsigned int*)(field.stat()->stat.maxs.value.data)) {
        *(unsigned int*)(field.stat()->stat.maxs.value.data) = value;
        tuple_set.reset(value, index);
      }
    }
    break;
    default: {
      LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
    }
  }
  return false;
}

// MIN运算
static bool min_tuple_add(TupleSet &tuple_set, const TupleField &field, \
  const FieldMeta *field_meta, Tuple &tuple, const char *record, int index) {
  // assert(field_meta != nullptr);
  assert(field.stat() != nullptr);
  Bitmap bitmap(const_cast<char*>(record), RECORD_BITMAP_BITS);
  if(bitmap.get_bit(field_meta->index())) return false;

  switch (field_meta->type()) {
    case INTS: {
      int value = *(int*)(record + field_meta->offset());
      if(tuple_set.is_empty()) {
        *(int*)(field.stat()->stat.mins.value.data) = value;
        tuple.add(value, INTS);
        return true;
      } else if(value < *(int*)(field.stat()->stat.mins.value.data)) {
        *(int*)(field.stat()->stat.mins.value.data) = value;
        tuple_set.reset(value, index);
      }
    }
    break;
    case FLOATS: {
      float value = *(float *)(record + field_meta->offset());
      if(tuple_set.is_empty()) {
        *(float*)(field.stat()->stat.mins.value.data) = value;
        tuple.add(value, FLOATS);
        return true;
      } else if(value < *(float*)(field.stat()->stat.mins.value.data)) {
        *(float*)(field.stat()->stat.mins.value.data) = value;
        tuple_set.reset(value, index);
      }
    }
      break;
    case CHARS: {
      const char *s = record + field_meta->offset();  // 现在当做Cstring来处理
      if(tuple_set.is_empty()) {
        free(field.stat()->stat.mins.value.data);
        value_init_string(&(field.stat()->stat.mins.value), s);
        tuple.add(s, strlen(s), CHARS);
        return true;
      } else if(0 > strcmp(s, (char*)(field.stat()->stat.mins.value.data))) {
          free(field.stat()->stat.mins.value.data);
          value_init_string(&(field.stat()->stat.mins.value), s);
          tuple_set.reset(s, strlen(s), index);
      }
    }
    break;
    case DATES: {
      unsigned int value = *(unsigned int*)(record + field_meta->offset()); 
      if(tuple_set.is_empty()) {
        *(unsigned int*)(field.stat()->stat.mins.value.data) = value;
        tuple.add(value, DATES);
        return true;
      } else if(value < *(unsigned int*)(field.stat()->stat.mins.value.data)) {
        *(unsigned int*)(field.stat()->stat.mins.value.data) = value;
        tuple_set.reset(value, index);
      }
    }
    break;
    default: {
      LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
    }
  }
  return false;
}

// COUNT运算
static bool count_tuple_add(TupleSet &tuple_set, const TupleField &field, \
  const FieldMeta *field_meta, Tuple &tuple, const char *record, int index) {
  // assert(field_meta != nullptr);
  assert(field.stat() != nullptr);
  Bitmap bitmap(const_cast<char*>(record), RECORD_BITMAP_BITS);

  bool include_null = false;

  if(field.field_name()[0] == '*' || (field.field_name()[0] >= '0' && field.field_name()[0] <= '9')) {
    include_null = true;
  }

  if(tuple_set.is_empty()) {
    if(include_null || !bitmap.get_bit(field_meta->index())) {
      field.stat()->stat.counts.count = 1;
    } else if(bitmap.get_bit(field_meta->index())) {
      field.stat()->stat.counts.count = 0;
    }
    tuple.add(field.stat()->stat.counts.count, INTS);
    return true;
  } else if(include_null || !bitmap.get_bit(field_meta->index())) {
    tuple_set.reset(++(field.stat()->stat.counts.count), index);
  }
  
  return false;
}

// AVG运算
static bool avg_tuple_add(TupleSet &tuple_set, const TupleField &field, \
  const FieldMeta *field_meta, Tuple &tuple, const char *record, int index) {
  // assert(field_meta != nullptr);
  assert(field.stat() != nullptr);
  Bitmap bitmap(const_cast<char*>(record), RECORD_BITMAP_BITS);
  if(bitmap.get_bit(field_meta->index())) return false;

  switch (field_meta->type()) {
    case INTS: {
      int value = *(int*)(record + field_meta->offset());
      if(tuple_set.is_empty()) {
        field.stat()->stat.avgs.count = 1;
        field.stat()->stat.avgs.sum = (float)(value);
        tuple.add((float)value, FLOATS);
        return true;
      } else {
        ++field.stat()->stat.avgs.count;
        field.stat()->stat.avgs.sum += (float)(value);
        tuple_set.reset(field.stat()->stat.avgs.sum/field.stat()->stat.avgs.count, index);
      }
    }
    break;
    case FLOATS: {
      float value = *(float *)(record + field_meta->offset());
      if(tuple_set.is_empty()) {
        field.stat()->stat.avgs.count = 1;
        field.stat()->stat.avgs.sum = value;
        tuple.add((float)value, FLOATS);
        return true;
      } else {
        ++field.stat()->stat.avgs.count;
        field.stat()->stat.avgs.sum += value;
        tuple_set.reset(field.stat()->stat.avgs.sum/field.stat()->stat.avgs.count, index);
      }
    }
    break;
    case CHARS: 
    case DATES: 
    default: {
      LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
    }
  }
  return false;
}

TupleRecordConverter::TupleRecordConverter(Table *table, TupleSet &tuple_set) :
      table_(table), tuple_set_(tuple_set){
}

void TupleRecordConverter::add_record(const char *record) {
  const TupleSchema &schema = tuple_set_.schema();
  Tuple tuple;
  const TableMeta &table_meta = table_->table_meta();
  int index = 0;
  bool rc = false;
  for (const TupleField &field : schema.fields()) {
    switch (field.atype()){
      case MAX_A: {
        const FieldMeta *field_meta = table_meta.field(field.field_name());
        rc = max_tuple_add(tuple_set_, field, field_meta, tuple, record, index);
      }
      break;
      case MIN_A: {
        const FieldMeta *field_meta = table_meta.field(field.field_name());
        rc = min_tuple_add(tuple_set_, field, field_meta, tuple, record, index);
      }
      break;
      case COUNT_A: {
        const FieldMeta *field_meta = table_meta.field(field.field_name());
        rc = count_tuple_add(tuple_set_, field, field_meta, tuple, record, index);
      }
      break;
      case AVG_A: {
        const FieldMeta *field_meta = table_meta.field(field.field_name());
        rc = avg_tuple_add(tuple_set_, field, field_meta, tuple, record, index);
      }
      break;
      case NOTHING_A: {
        const FieldMeta *field_meta = table_meta.field(field.field_name());
        rc = normal_tuple_add(field_meta, tuple, record, table_);
      }
      break;
    }
    index++;
  }

  if(rc) tuple_set_.add(std::move(tuple));
}


