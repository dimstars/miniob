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
// Created by Longda on 2021/4/13.
//
#ifndef __OBSERVER_STORAGE_COMMON_RECORD_MANAGER_H_
#define __OBSERVER_STORAGE_COMMON_RECORD_MANAGER_H_

#include "storage/default/disk_buffer_pool.h"

typedef int SlotNum;
struct PageHeader;
struct OFPageHeader;
class ConditionFilter;

// text类型在record的存放内容
struct OID{
  int len;
  PageNum first_page_num;
};

struct RID 
{
  PageNum page_num; // record's page number
  SlotNum slot_num; // record's slot number // page内record的位置
  // bool    valid;    // true means a valid record

  bool operator== (const RID &other) const {
    return page_num == other.page_num && slot_num == other.slot_num;
  }
};

class RidDigest {
public:
  size_t operator() (const RID &rid) const {
    return ((size_t)(rid.page_num) << 32) | rid.slot_num;
  }
};

struct Record 
{
  // bool valid; // false means the record hasn't been load
  RID  rid;   // record's rid
  char *data; // record's data
};

class RecordPageHandler {
public:
  RecordPageHandler();
  ~RecordPageHandler();
  RC init(DiskBufferPool &buffer_pool, const char *file_name, PageNum page_num);
  RC init_empty_page(DiskBufferPool &buffer_pool, const char *file_name, PageNum page_num, int record_size);
  RC deinit();

  RC insert_record(const char *data, RID *rid);
  RC update_record(const Record *rec);

  template <class RecordUpdater>
  RC update_record_in_place(const RID *rid, RecordUpdater updater) {
    Record record;
    RC rc = get_record(rid, &record);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    rc = updater(record);
    disk_buffer_pool_->mark_dirty(&page_handle_);
    return rc;
  }

  RC delete_record(const RID *rid);

  RC get_record(const RID *rid, Record *rec);
  RC get_first_record(Record *rec);
  RC get_next_record(Record *rec);

  PageNum get_page_num() const;

  bool is_full() const;

private:
  DiskBufferPool * disk_buffer_pool_;
  std::string      file_name_;
  BPPageHandle     page_handle_;
  PageHeader    *  page_header_;
  char *           bitmap_;       // 每个普通page内的bitmap
};

class OverflowPageHandler {
public:
  OverflowPageHandler();
  ~OverflowPageHandler();

  RC init(DiskBufferPool &buffer_pool, const char *file_name, PageNum page_num);
  RC deinit();

  RC insert_record(const char *data, int data_len, PageNum next_page_num);
  RC update_record();

  RC delete_record();

  RC get_record(char * &data);

  PageNum get_page_num() const;
  PageNum get_next_page_num() const;

private:
  DiskBufferPool * disk_buffer_pool_;
  std::string      file_name_;
  BPPageHandle     page_handle_;    
  OFPageHeader   * page_header_;
};

class RecordFileHandler {
public:
  RecordFileHandler();
  RC init(DiskBufferPool &buffer_pool, const char *file_name);
  void close();

  /**
   * 更新指定文件中的记录，rec指向的记录结构中的rid字段为要更新的记录的标识符，
   * pData字段指向新的记录内容
   * @param rec
   * @return
   */
  RC update_record(const Record *rec);

  /**
   * 从指定文件中删除标识符为rid的记录
   * @param rid
   * @return
   */
  RC delete_record(const RID *rid);

  /**
   * 插入一个新的记录到指定文件中，pData为指向新纪录内容的指针，返回该记录的标识符rid
   * @param data
   * @param rid
   * @return
   */
  RC insert_record(const char *data, int record_size, RID *rid);

  /**
   * 获取指定文件中标识符为rid的记录内容到rec指向的记录结构中
   * @param rid
   * @param rec
   * @return
   */
  RC get_record(const RID *rid, Record *rec);

  template<class RecordUpdater> // TODO 改成普通模式, 不使用模板
  RC update_record_in_place(const RID *rid, RecordUpdater updater) {

    RC rc = RC::SUCCESS;
    RecordPageHandler page_handler;
    if ((rc != page_handler.init(*disk_buffer_pool_, file_name_.c_str(), rid->page_num)) != RC::SUCCESS) {
      return rc;
    }

    return page_handler.update_record_in_place(rid, updater);
  }

private:
  DiskBufferPool  *   disk_buffer_pool_;
  std::string         file_name_;

  RecordPageHandler   record_page_handler_;        // 目前只有insert record使用
};

class OverflowFileHandler {
public:
  OverflowFileHandler();
  RC init(DiskBufferPool &buffer_pool, const char *file_name);
  void close();

  /**
   * 更新指定文件中的记录，rec指向的记录结构中的rid字段为要更新的记录的标识符，
   * pData字段指向新的记录内容
   * @param rec
   * @return
   */
  RC update_record(const char *data, int data_len, const OID *old_oid, OID *new_oid);

  /**
   * 从指定文件中删除标识符为rid的记录
   * @param rid
   * @return
   */
  RC delete_record(const OID *rid);

  /**
   * 插入一个新的记录到指定文件中，pData为指向新纪录内容的指针，返回该记录的标识符rid
   * @param data
   * @param rid
   * @return
   */
  RC insert_record(const char *data, int data_len, OID *oid);

  /**
   * 获取指定文件中标识符为rid的记录内容到rec指向的记录结构中
   * @param rid
   * @param rec
   * @return
   */
  RC get_record(const OID *oid, char *data);


private:
  DiskBufferPool  *   disk_buffer_pool_;
  std::string         file_name_;
};

class RecordFileScanner 
{
public:
  RecordFileScanner();

  /**
   * 打开一个文件扫描。
   * 本函数利用从第二个参数开始的所有输入参数初始化一个由参数rmFileScan指向的文件扫描结构，
   * 在使用中，用户应先调用此函数初始化文件扫描结构，
   * 然后再调用GetNextRec函数来逐个返回文件中满足条件的记录。
   * 如果条件数量conNum为0，则意味着检索文件中的所有记录。
   * 如果条件不为空，则要对每条记录进行条件比较，只有满足所有条件的记录才被返回
   * @param buffer_pool 
   * @param file_name 
   * @param condition_num 
   * @param conditions
   * @return
   */
  RC open_scan(DiskBufferPool & buffer_pool, const char *file_name, ConditionFilter *condition_filter);

  /**
   * 关闭一个文件扫描，释放相应的资源
   * @return
   */
  RC close_scan();

  RC get_first_record(Record *rec);

  /**
   * 获取下一个符合扫描条件的记录。
   * 如果该方法成功，返回值rec应包含记录副本及记录标识符。
   * 如果没有发现满足扫描条件的记录，则返回RM_EOF
   * @param rec 上一条记录。如果为NULL，就返回第一条记录
   * @return
   */
  RC get_next_record(Record *rec);

private:
  DiskBufferPool  *   disk_buffer_pool_;
  std::string         file_name_;

  ConditionFilter *   condition_filter_;
  RecordPageHandler   record_page_handler_;
};



#endif //__OBSERVER_STORAGE_COMMON_RECORD_MANAGER_H_