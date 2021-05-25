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
// Created by Wangyunlai on 2021/5/24.
//

#ifndef __OBSERVER_STORAGE_TRX_TRX_H_
#define __OBSERVER_STORAGE_TRX_TRX_H_

#include <stddef.h>
#include <unordered_map>
#include <unordered_set>

#include "handler/handler_defs.h"
#include "storage/common/record_manager.h"
#include "rc.h"

class Table;

class Operation {
public:
  enum class Type: int {
    INSERT,
    UPDATE,
    DELETE,
    UNDEFINED,
  };

public:
  Operation(Type type, const RID &rid) : type_(type), page_num_(rid.page_num), slot_num_(rid.slot_num){
  }

  Type type() const {
    return type_;
  }
  PageNum  page_num() const {
    return page_num_;
  }
  SlotNum  slot_num() const {
    return slot_num_;
  }

private:
  Type type_;
  PageNum  page_num_;
  SlotNum  slot_num_;
};
class OperationHasher {
public:
  size_t operator() (const Operation &op) const {
    return (((size_t)op.page_num()) << 32) | (op.slot_num());
  }
};

class OperationEqualer {
public:
  bool operator()(const Operation &op1, const Operation &op2) const {
    return op1.page_num() == op2.page_num() &&
           op1.slot_num() == op2.slot_num();
  }
};

class Trx {
public:
  static int32_t default_trx_id();
  static int32_t next_trx_id();
  static const char *trx_field_name();
  static AttrType trx_field_type();
  static int      trx_field_len();

public:

public:
  RC insert_record(Table *table, Record *record);
  RC delete_record(Table *table, Record *record);

  RC commit();
  RC rollback();

  RC commit_insert(Table *table, Record &record);
  RC rollback_delete(Table *table, Record &record);

  bool is_visible(Table *table, const Record *record);

private:
  void set_record_trx_id(Table *table, Record *record, int32_t trx_id, bool deleted) const;
  static void get_record_trx_id(Table *table, const Record *record, int32_t &trx_id, bool &deleted);

private:
  using OperationSet = std::unordered_set<Operation, OperationHasher, OperationEqualer>;

  Operation *find_operation(Table *table, const RID &rid);
  void insert_operation(Table *table, Operation::Type type, const RID &rid);
  void delete_operation(Table *table, const RID &rid);

private:
  int32_t  trx_id_;
  std::unordered_map<Table *, OperationSet> operations_; // TODO set
};

#endif // __OBSERVER_STORAGE_TRX_TRX_H_
