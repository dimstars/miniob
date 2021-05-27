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

#include <atomic>

#include "storage/trx/trx.h"
#include "storage/lock/lock.h"
#include "storage/common/table.h"
#include "storage/common/record_manager.h"
#include "storage/common/field_meta.h"
#include "common/log/log.h"

static const uint32_t DELETED_FLAG_BIT_MASK = 0x80000000;
static const uint32_t TRX_ID_BIT_MASK = 0x7FFFFFFF;

int32_t Trx::default_trx_id() {
  return 0;
}

int32_t Trx::next_trx_id() {
  static std::atomic<int32_t> trx_id;
  return ++trx_id;
}

const char *Trx::trx_field_name() {
  return "__trx";
}

AttrType Trx::trx_field_type() {
  return ints;
}

int Trx::trx_field_len() {
  return sizeof(int32_t);
}

Trx::Trx() {
  table_locks_ = new TableLocksInTrx(this);
  record_locks_ = new RecordLocksInTrx(this);
}

Trx::~Trx() {
  delete table_locks_;
  table_locks_ = nullptr;
  delete record_locks_;
  record_locks_ = nullptr;
}

RC Trx::insert_record(Table *table, Record *record) {
  RC rc = RC::SUCCESS;
  // 先校验是否以前是否存在过(应该不会存在) TODO
  Operation *old_oper = find_operation(table, record->rid);
  if (old_oper != nullptr) {
    return RC::GENERIC_ERROR; // TODO error code
  }

  start_if_not_started();

  // 设置record中trx_field为当前的事务号
  // set_record_trx_id(table, record, trx_id_, false);
  // 记录到operations中
  insert_operation(table, Operation::Type::INSERT, record->rid);
  return rc;
}

RC Trx::delete_record(Table *table, Record *record) {
  RC rc = RC::SUCCESS;
  start_if_not_started();
  Operation *old_oper = find_operation(table, record->rid);
  if (old_oper != nullptr) {
    if (old_oper->type() == Operation::Type::INSERT) {
      delete_operation(table, record->rid);
      return RC::SUCCESS;
    } else {
      return RC::GENERIC_ERROR; // TODO error code
    }
  }
  set_record_trx_id(table, *record, trx_id_, true);
  insert_operation(table, Operation::Type::DELETE, record->rid);
  return rc;
}

void Trx::set_record_trx_id(Table *table, Record &record, int32_t trx_id, bool deleted) const {
  const FieldMeta *trx_field = table->table_meta().trx_field();
  int32_t *ptrx_id = (int32_t*)(record.data + trx_field->offset());
  if (deleted) {
    trx_id |= DELETED_FLAG_BIT_MASK;
  }
  *ptrx_id = trx_id;
}

void Trx::get_record_trx_id(Table *table, const Record &record, int32_t &trx_id, bool &deleted) {
  const FieldMeta *trx_field = table->table_meta().trx_field();
  int32_t trx = *(int32_t*)(record.data + trx_field->offset());
  trx_id = trx & TRX_ID_BIT_MASK;
  deleted = (trx & DELETED_FLAG_BIT_MASK) != 0;
}

Operation *Trx::find_operation(Table *table, const RID &rid) {
  std::unordered_map<Table *, OperationSet>::iterator table_operations_iter = operations_.find(table);
  if (table_operations_iter == operations_.end()) {
    return nullptr;
  }

  OperationSet & table_operations = table_operations_iter->second;
  Operation tmp(Operation::Type::UNDEFINED, rid);
  OperationSet::iterator operation_iter = table_operations.find(tmp);
  if (operation_iter == table_operations.end()) {
    return nullptr;
  }
  return const_cast<Operation *>(&(*operation_iter));
}

void Trx::insert_operation(Table *table, Operation::Type type, const RID &rid) {
  OperationSet & table_operations = operations_[table];
  table_operations.emplace(type, rid);
}

void Trx::delete_operation(Table *table, const RID &rid) {

  std::unordered_map<Table *, OperationSet>::iterator table_operations_iter = operations_.find(table);
  if (table_operations_iter == operations_.end()) {
    return ;
  }

  Operation tmp(Operation::Type::UNDEFINED, rid);
  table_operations_iter->second.erase(tmp);
}

RC Trx::commit() {
  RC rc = RC::SUCCESS;
  for (const auto &table_operations: operations_) {
    Table *table = table_operations.first;
    const OperationSet &operation_set = table_operations.second;
    for (const Operation &operation: operation_set) {

      RID rid;
      rid.page_num = operation.page_num();
      rid.slot_num = operation.slot_num();

      switch (operation.type()) {
        case Operation::Type::INSERT: {
          rc = table->commit_insert(this, rid);
          if (rc != RC::SUCCESS) {
            // TODO handle rc
            LOG_ERROR("Failed to commit insert operation. rid=%d.%d, rc=%d:%s",
                      rid.page_num, rid.slot_num, rc, strrc(rc));
          }
        }
        break;
        case Operation::Type::DELETE: {
          LockManager::instance().delete_record(table, this, rid);
          rc = table->commit_delete(this, rid);
          if (rc != RC::SUCCESS) {
            // TODO handle rc
            LOG_ERROR("Failed to commit delete operation. rid=%d.%d, rc=%d:%s",
                      rid.page_num, rid.slot_num, rc, strrc(rc));
          }
        }
        break;
        default: {
          LOG_PANIC("Unknown operation. type=%d", (int)operation.type());
        }
        break;
      }
    }
  }

  operations_.clear();
  trx_id_ = 0;
  LockManager::instance().unlock(this);
  return rc;
}

RC Trx::rollback() {
  RC rc = RC::SUCCESS;
  for (const auto &table_operations: operations_) {
    Table *table = table_operations.first;
    const OperationSet &operation_set = table_operations.second;
    for (const Operation &operation: operation_set) {

      RID rid;
      rid.page_num = operation.page_num();
      rid.slot_num = operation.slot_num();

      switch (operation.type()) {
        case Operation::Type::INSERT: {
          rc = table->rollback_insert(this, rid);
          if (rc != RC::SUCCESS) {
            // TODO handle rc
            LOG_ERROR("Failed to rollback insert operation. rid=%d.%d, rc=%d:%s",
                      rid.page_num, rid.slot_num, rc, strrc(rc));
          }
        }
          break;
        case Operation::Type::DELETE: {
          rc = table->rollback_delete(this, rid);
          if (rc != RC::SUCCESS) {
            // TODO handle rc
            LOG_ERROR("Failed to rollback delete operation. rid=%d.%d, rc=%d:%s",
                      rid.page_num, rid.slot_num, rc, strrc(rc));
          }
        }
          break;
        default: {
          LOG_PANIC("Unknown operation. type=%d", (int)operation.type());
        }
          break;
      }
    }
  }

  operations_.clear();
  trx_id_ = 0;
  LockManager::instance().unlock(this);
  return rc;
}

RC Trx::commit_insert(Table *table, Record &record) {
  set_record_trx_id(table, record, 0, false);
  return RC::SUCCESS;
}

RC Trx::rollback_delete(Table *table, Record &record) {
  set_record_trx_id(table, record, 0, false);
  return RC::SUCCESS;
}

bool Trx::is_visible(Table *table, const Record *record) {
  int32_t record_trx_id;
  bool record_deleted;
  get_record_trx_id(table, *record, record_trx_id, record_deleted);

  // 0 表示这条数据已经提交
  if (0 == record_trx_id || record_trx_id == trx_id_) {
    return !record_deleted;
  }

  return record_deleted; // 当前记录上面有事务号，说明是未提交数据，那么如果有删除标记的话，就表示是未提交的删除
}

void Trx::init_trx_info(Table *table, Record &record) {
  set_record_trx_id(table, record, trx_id_, false);
}

void Trx::start_if_not_started() {
  if (trx_id_ == 0) {
    trx_id_ = next_trx_id();
  }
}

TableLocksInTrx &Trx::table_locks() {
  return *table_locks_;
}
RecordLocksInTrx &Trx::record_locks() {
  return *record_locks_;
}

void Trx::wait(std::unique_lock<std::mutex> &mutex) {
  lock_waiter_.wait(mutex);
}

void Trx::wakeup() {
  lock_waiter_.notify_one();
}