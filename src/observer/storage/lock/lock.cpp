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
// Created by wangyunlai.wyl on 2021/5/25.
//

#include "storage/lock/lock.h"

#include <memory>
#include "storage/trx/trx.h"
#include "storage/common/table.h"
#include "common/log/log.h"

static const bool TABLE_LOCK_HIGHER_LEVEL[4][4] = {
        // SHARED, EXCLUSIVE, INTENT_SHARED, INTENT_EXCLUSIVE
        {true,      false,     true,          false},   // SHARED
        {true,      true,      true,          true},    // EXCLUSIVE
        {false,     false,     true,          false},   // INTENT_SHARED,
        {false,     false,     true,          true}     // INTENT_EXCLUSIVE
};

static const bool TABLE_LOCK_COMPATIBLE_TABLE[4][4] = {
        // SHARED, EXCLUSIVE, INTENT_SHARED, INTENT_EXCLUSIVE
        {  true,   false,     false,         false},    // SHARED
        {  false,  false,     false,         false},    // EXCLUSIVE
        {  true,   false,     true,          true},     // INTENT_SHARED,
        {  false,  false,     true,          true}      // INTENT_EXCLUSIVE
};

static const bool RECORD_LOCK_HIGHER_LEVEL[2][2] = {
        // SHARED, EXCLUSIVE
        {  true,   false},   // SHARED
        {  true,   true},    // EXCLUSIVE
};

static const bool RECORD_LOCK_COMPATIBLE_TABLE[2][2] = {
        // SHARED, EXCLUSIVE
        {  true,   false},    // SHARED
        {  false,  false}     // EXCLUSIVE
};

////////////////////////////////////////////////////////////////////////////////
LockMeta::LockMeta(LockTargetType type, Table *table, Trx *trx):
    target_type_(type), table_(table), trx_(trx) {
}

LockTargetType LockMeta::target_type() const {
  return target_type_;
}

Table *LockMeta::table() const {
  return table_;
}

Trx *LockMeta::trx() const {
  return trx_;
}

////////////////////////////////////////////////////////////////////////////////
LockTableMeta::LockTableMeta(Table *table, Trx *trx, LockTableType type) :
        LockMeta(LockTargetType::TABLE, table, trx), table_type_(type) {
}

LockTableType LockTableMeta::table_type() const {
  return table_type_;
}

void LockTableMeta::set_table_type(LockTableType type) {
  table_type_ = type;
}

////////////////////////////////////////////////////////////////////////////////
LockRecordMeta::LockRecordMeta(Table *table, Trx *trx, const RID &rid, LockRecordType type) :
      LockMeta(LockTargetType::RECORD, table, trx), record_type_(type), rid_(rid) {
}

LockRecordType LockRecordMeta::record_type() const {
  return record_type_;
}

const RID &LockRecordMeta::rid() const {
  return rid_;
}

void LockRecordMeta::set_record_type(LockRecordType type) {
  record_type_ = type;
}

////////////////////////////////////////////////////////////////////////////////
TableLocksInTrx::TableLocksInTrx(Trx *trx) : trx_(trx) {
}

bool TableLocksInTrx::is_locked(Table *table, LockTableType type) {
  // 判断当前是否包含了这个锁或者有更高级别的锁
  const auto iter = locks_.find(table);
  if (iter == locks_.end()) {
    return false;
  }
  const LockTableMeta & lock_table_meta = iter->second;
  LockTableType table_type = lock_table_meta.table_type();
  return TABLE_LOCK_HIGHER_LEVEL[(int)table_type][(int)type];
}

void TableLocksInTrx::add_lock(Table *table, LockTableType type) {
  auto iter = locks_.find(table);
  if (iter != locks_.end()) {
    LockTableMeta & lock_table_meta = iter->second;
    if (TABLE_LOCK_HIGHER_LEVEL[(int)type][(int)lock_table_meta.table_type()]) {
      lock_table_meta.set_table_type(type);
    }
  } else {
    locks_.emplace(table, LockTableMeta(table, trx_, type));
  }
}

void TableLocksInTrx::unlock() {
  for (const auto &table_lock: locks_) {
    Table *table = table_lock.first;
    TableLocksInTable &table_locks_in_table = table->table_locks();
    table_locks_in_table.unlock(trx_);
  }
  locks_.clear();
}

////////////////////////////////////////////////////////////////////////////////
RecordLocksInTrx::RecordLocksInTrx(Trx *trx) : trx_(trx){
}

bool RecordLocksInTrx::is_locked(Table *table, const RID &rid, LockRecordType type) {
  const auto &table_locks_iter = locks_.find(table);
  if (table_locks_iter == locks_.end()) {
    return false;
  }

  const auto &record_lock_iter = table_locks_iter->second.find(rid);
  if (record_lock_iter == table_locks_iter->second.end()) {
    return false;
  }

  const LockRecordMeta & record_lock_meta = record_lock_iter->second;
  LockRecordType record_type = record_lock_meta.record_type();
  return RECORD_LOCK_HIGHER_LEVEL[(int)record_type][(int)type];
}

void RecordLocksInTrx::add_lock(Table *table, const RID &rid, LockRecordType type) {
  RecordLockMap &record_locks = locks_[table];
  auto record_lock_iter = record_locks.find(rid);
  if (record_lock_iter != record_locks.end()) {
    LockRecordMeta &record_lock = record_lock_iter->second;
    if (type == LockRecordType::EXCLUSIVE  && record_lock.record_type() != LockRecordType::EXCLUSIVE) {
      record_lock.set_record_type(type);
    }
  } else {
    record_locks.emplace(rid, LockRecordMeta(table, trx_, rid, type));
  }
}
void RecordLocksInTrx::unlock() {
  for (const auto &record_locks: locks_) {
    Table *table = record_locks.first;
    RecordLocksInTable &record_locks_in_table = table->record_locks();

    for (const auto &record_lock: record_locks.second) {
      record_locks_in_table.unlock(record_lock.first, trx_);
    }
  }
  locks_.clear();
}

////////////////////////////////////////////////////////////////////////////////
TableLocksInTable::TableLocksInTable(Table *table) : table_(table){
  mutex_ptr_ = std::make_shared<std::mutex>();
}

void TableLocksInTable::lock(Trx *trx, LockTableType type) {
  std::unique_lock<std::mutex> unique_lock(*mutex_ptr_);

  auto table_lock_iter = locks_.find(trx);
  if (locks_.end() != table_lock_iter) {
    LockTableMeta &table_lock = table_lock_iter->second;
    LockTableType table_type = table_lock.table_type();
    if (TABLE_LOCK_HIGHER_LEVEL[(int)table_type][(int)type]) {
      return ;
    }

    // need upgrade
    // 只有一个锁，不管waiting_list直接升级
    if (locks_.size() == 1) {
      table_lock.set_table_type(type);
      return;
    }

    // 不只有一个锁，判断是否除了自己全部兼容
    bool all_compatible = true;
    for (const auto &lock: locks_) {
      const LockTableMeta & table_meta = lock.second;
      if (table_meta.trx() == trx) {
        continue;
      }
      if (!TABLE_LOCK_COMPATIBLE_TABLE[(int)type][(int)table_meta.table_type()]) {
        all_compatible = false;
        break;
      }
    }

    if (all_compatible) {
      table_lock.set_table_type(type);
      return ;
    }

    waiting_list_.emplace_back(table_, trx, type);
    trx->wait(unique_lock);
    lock(trx, type); // TODO 递归调用
    remove_from_waiting_list(trx);
    wakeup_first_lock();
    return;
  }

  if (!waiting_list_.empty()) {
    waiting_list_.emplace_back(table_, trx, type); // 有可能已经在waiting_list中了 TODO
    trx->wait(unique_lock);
    lock(trx, type);
    remove_from_waiting_list(trx);
    wakeup_first_lock();
  } else {
    // 判断是否都兼容
    bool all_compatible = true;
    for (const auto &lock : locks_) {
      LockTableType table_type = lock.second.table_type();
      if (!TABLE_LOCK_COMPATIBLE_TABLE[(int)type][(int)table_type]) {
        all_compatible = false;
        waiting_list_.emplace_back(table_, trx, type);
        break;
      }
    }
    if (all_compatible) {
      locks_.emplace(trx, LockTableMeta(table_, trx, type));
    } else {
      trx->wait(unique_lock);
      lock(trx, type); // TODO 消除递归
      remove_from_waiting_list(trx);
      wakeup_first_lock();
    }
  }
}

void TableLocksInTable::unlock(Trx *trx) {
  std::unique_lock<std::mutex> unique_lock(*mutex_ptr_);

  auto table_lock_iter = locks_.find(trx);
  if (table_lock_iter == locks_.end()) {
    LOG_PANIC("Cannot find lock of trx while unlock");
    return;
  }

  locks_.erase(table_lock_iter);

  // 不做多余判断，直接唤醒等待队列中第一个锁
  wakeup_first_lock();
}

void TableLocksInTable::remove_from_waiting_list(Trx *trx) {
  for (auto iter = waiting_list_.begin(), iter_end = waiting_list_.end();
        iter != iter_end; ++iter) {
    LockTableMeta &table_lock = *iter;
    if (trx == table_lock.trx()) {
      waiting_list_.erase(iter);
      break;
    }
  }
}

void TableLocksInTable::wakeup_first_lock() {
  if (waiting_list_.empty()) {
    return;
  }

  waiting_list_.front().trx()->wakeup();
}
////////////////////////////////////////////////////////////////////////////////
RecordLocksInRecord::RecordLocksInRecord(Table *table, const RID &rid) : table_(table), rid_(rid) {
  mutex_ptr_ = std::make_shared<std::mutex>();
}

void RecordLocksInRecord::lock(Trx *trx, LockRecordType type) {
  RC rc = RC::SUCCESS;
  std::unique_lock<std::mutex> unique_lock(*mutex_ptr_);
  do {
    rc = lock_internal(trx, type);
    if (rc == RC::LOCKED_NEED_WAIT) {
      trx->wait(unique_lock);
    }
  } while (rc != RC::SUCCESS);
}

RC RecordLocksInRecord::lock_internal(Trx *trx, LockRecordType type) {
  RC rc = RC::SUCCESS;

  auto record_lock_iter = locks_.find(trx);
  if (record_lock_iter != locks_.end()) {
    LockRecordMeta & record_lock = record_lock_iter->second;
    if (RECORD_LOCK_HIGHER_LEVEL[(int)record_lock.record_type()][(int)type]) { // 当前事务已经有足够高级别的锁
      return rc;
    }

    // 有了锁，但是需要升级
    if (locks_.size() == 1) { // 只有当前这个事务加了锁，可以升级(有人排队也不管)。也只有这种场景可以升级
      record_lock.set_record_type(type);
      return rc;
    }

    // 不能升级
    waiting_list_.emplace_back(table_, trx, rid_, type);
    rc = RC::LOCKED_NEED_WAIT;
  } else {
    // 当前事务没有这个行的锁，查看是否完全兼容
    if (!waiting_list_.empty()) {
      waiting_list_.emplace_back(table_, trx, rid_, type);
      rc = RC::LOCKED_NEED_WAIT;
    } else {
      bool all_compatible = true;
      for (const auto &record_lock_pair: locks_) {
        const LockRecordMeta &record_lock = record_lock_pair.second;
        if (!RECORD_LOCK_COMPATIBLE_TABLE[(int)record_lock.record_type()][(int)type]) {
          all_compatible = false;
          break;
        }
      }
      if (all_compatible) {
        locks_.emplace(trx, LockRecordMeta(table_, trx, rid_, type));
      } else {
        waiting_list_.emplace_back(table_, trx, rid_, type);
        rc = RC::LOCKED_NEED_WAIT;
      }
    }
  }

  return rc;
}

void RecordLocksInRecord::unlock(Trx *trx) {
  std::unique_lock<std::mutex> unique_lock(*mutex_ptr_);

  if (locks_.size() == 1) {
    // assert locks_[0].trx() == trx
    locks_.clear();
    // wakeup 后面等待队列中的锁
    if (waiting_list_.empty()) {
      return;
    }

    LockRecordMeta &record_lock = waiting_list_.front();
    if (record_lock.record_type() == LockRecordType::EXCLUSIVE) {
      locks_.emplace(record_lock.trx(), record_lock); // 直接加锁成功
      record_lock.trx()->wakeup();
      waiting_list_.erase(waiting_list_.begin());
    } else { //SHARED 类型的锁，需要唤醒多个

      for (auto waiting_iter = waiting_list_.begin(); waiting_iter != waiting_list_.end(); ) {
        LockRecordMeta &waiting_record_lock = *waiting_iter;
        if (waiting_record_lock.record_type() != LockRecordType::SHARED) {
          break;
        }

        locks_.emplace(waiting_record_lock.trx(), waiting_record_lock);
        waiting_record_lock.trx()->wakeup();
        waiting_iter = waiting_list_.erase(waiting_iter);
      }
    }
    return;
  }

  // 当前已经有多个锁 , 肯定都是SHARED类型锁
  locks_.erase(trx);
  // 如果只剩下一个，那么就可以考虑是否可以唤醒waiting_list中的第一个元素，因为第一个可能跟剩下的那个锁是同一个事务的
  if (locks_.size() == 1 && !waiting_list_.empty()) {
    LockRecordMeta &lock_record = waiting_list_.front();
    if (lock_record.trx() == locks_.begin()->first) {
      // 等待队列中的这个锁肯定是排他锁， 升级成功了
      locks_.begin()->second.set_record_type(lock_record.record_type());
      lock_record.trx()->wakeup();
      waiting_list_.erase(waiting_list_.begin());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
RecordLocksInTable::RecordLocksInTable(Table *table) : table_(table) {
}

void RecordLocksInTable::lock(Trx *trx, const RID &rid, LockRecordType type) {
  auto record_locks_iter = locks_.find(rid);
  if (record_locks_iter == locks_.end()) {
    record_locks_iter = locks_.insert(std::make_pair(rid, RecordLocksInRecord(table_, rid))).first;
  }
  record_locks_iter->second.lock(trx, type);
}

void RecordLocksInTable::unlock(const RID &rid, Trx *trx) {
  auto record_locks_iter = locks_.find(rid);
  if (record_locks_iter == locks_.end()) {
    LOG_PANIC("Cannot find lock of rid=%d.%d", rid.page_num, rid.slot_num);
    return;
  }
  record_locks_iter->second.unlock(trx);
}
////////////////////////////////////////////////////////////////////////////////
LockManager::LockManager() {
}

LockManager &LockManager::instance() {
  static LockManager instance;
  return instance;
}

void LockManager::lock_record_shared(Table *table, Trx *trx, const RID &rid) {
  lock_record(table, trx, rid, LockRecordType::SHARED);
}

void LockManager::lock_record_exclusive(Table *table, Trx *trx, const RID &rid) {
  lock_record(table, trx, rid, LockRecordType::EXCLUSIVE);
}

void LockManager::lock_record(Table *table, Trx *trx, const RID &rid, LockRecordType type) {
  RecordLocksInTrx & record_locks_in_trx = trx->record_locks();
  if (record_locks_in_trx.is_locked(table, rid, type)) {
    return;
  }
  RecordLocksInTable & record_locks = table->record_locks();
  record_locks.lock(trx, rid, type);
  trx->record_locks().add_lock(table, rid, type);
}

void LockManager::unlock(Trx *trx) {
  // TODO table lock
  RecordLocksInTrx &record_locks_in_trx = trx->record_locks();
  record_locks_in_trx.unlock();
}
