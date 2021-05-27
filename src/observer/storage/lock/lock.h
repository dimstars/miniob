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

#ifndef __OBSERVER_STORAGE_LOCK_LOCK_H_
#define __OBSERVER_STORAGE_LOCK_LOCK_H_

#include <unordered_map>
#include <vector>
#include <mutex>

#include "storage/common/record_manager.h"
#include "rc.h"

class Table;
class Trx;

enum class LockTargetType {
  TABLE,
  RECORD,
};

enum class LockTableType : int {
  SHARED,
  EXCLUSIVE,
  INTENT_SHARED,
  INTENT_EXCLUSIVE,
};

enum class LockRecordType : int {
  SHARED,
  EXCLUSIVE,
};

class LockMeta {
public:
  LockMeta(LockTargetType type, Table *table, Trx *trx);
  virtual ~LockMeta() = default;

  LockTargetType target_type() const;
  Table *table() const;
  Trx *trx() const;
private:
  LockTargetType target_type_;
  Table *table_;
  Trx *trx_;
};

class LockTableMeta : public LockMeta {
public:
  LockTableMeta(Table *table, Trx *trx, LockTableType type);
  LockTableType table_type() const;
  void set_table_type(LockTableType type);
private:
  LockTableType table_type_;
};

class LockRecordMeta : public LockMeta {
public:
  LockRecordMeta(Table *table, Trx *trx, const RID &rid, LockRecordType type);

  LockRecordType record_type() const;
  void set_record_type(LockRecordType type);
  const RID &rid() const;

private:
  LockRecordType  record_type_;
  RID rid_;
};

class TableLocksInTrx {
public:
  explicit TableLocksInTrx(Trx *trx);
  /**
   * 给表加指定类型的锁
   */
  bool is_locked(Table *table, LockTableType type);
  void add_lock(Table *table, LockTableType type);
  void unlock();
private:
  Trx *trx_;
  std::unordered_map<Table *, LockTableMeta>  locks_;
};

class RecordLocksInTrx {
public:
  explicit RecordLocksInTrx(Trx *trx);

  /**
   * 判断某事务在某张表上是否已经有了符合要求的行锁
   */
  bool is_locked(Table *table, const RID &rid, LockRecordType type);
  void add_lock(Table *table, const RID &rid, LockRecordType type);
  /**
   * 某行被删除时，当前事务提交后，需要释放锁，并唤醒所有其它等待的锁
   */
  void delete_record(Table *table, const RID &rid);
  void unlock();

private:
  Trx *trx_;
  using RecordLockMap = std::unordered_map<RID, LockRecordMeta, RidDigest>;
  std::unordered_map<Table *, RecordLockMap> locks_;
};

class TableLocksInTable {
public:
  explicit TableLocksInTable(Table *table);
  void lock(Trx *trx, LockTableType type);
  void unlock(Trx *trx);

private:
  RC lock_internal(Trx *trx, LockTableType type);
  void remove_from_waiting_list(Trx *trx);
  void wakeup_first_lock();
private:
  std::shared_ptr<std::mutex> mutex_ptr_;
  Table *table_;
  std::unordered_map<Trx *, LockTableMeta> locks_;
  std::vector<LockTableMeta> waiting_list_;
};

class RecordLocksInRecord {
public:
  explicit RecordLocksInRecord(Table *table, const RID &rid);
  RC lock(Trx *trx, LockRecordType type);
  void unlock(Trx *trx);
  void delete_record(Trx *trx);
private:
  RC lock_internal(Trx *trx, LockRecordType type);

private:
  std::shared_ptr<std::mutex> mutex_ptr_;
  Table *table_;
  RID rid_;
  bool deleted_ = false;
  std::unordered_map<Trx *, LockRecordMeta> locks_;
  std::vector<LockRecordMeta> waiting_list_;
};

class RecordLocksInTable {
public:
  explicit RecordLocksInTable(Table *table);
  RC lock(Trx *trx, const RID &rid, LockRecordType type);
  void unlock(const RID &rid, Trx *trx);
  void delete_record(const RID &rid, Trx *trx);
private:
  Table *table_;
  std::unordered_map<RID, RecordLocksInRecord, RidDigest> locks_;
};

class LockManager {
public:
  LockManager();

  static LockManager &instance();
  void lock_table_shared(Table *table, Trx *trx);
  void lock_table_exclusive(Table *table, Trx *trx);
  void lock_table_intent_shared(Table *table, Trx *trx);
  void lock_table_intent_exclusive(Table *table, Trx *trx);
  RC lock_record_shared(Table *table, Trx *trx, const RID &rid);
  RC lock_record_exclusive(Table *table, Trx *trx, const RID &rid);
  void delete_record(Table *table, Trx *trx, const RID &rid);
  void unlock(Trx *trx);
private:
  void lock_table(Table *table, Trx *trx, LockTableType type);
  RC lock_record(Table *table, Trx *trx, const RID &rid, LockRecordType type); // TODO 行被删掉怎么处理
};

#endif //__OBSERVER_STORAGE_LOCK_LOCK_H_
