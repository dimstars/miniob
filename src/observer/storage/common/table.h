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
// Created by Wangyunlai on 2021/5/12.
//

#ifndef __OBSERVER_STORAGE_COMMON_TABLE_H__
#define __OBSERVER_STORAGE_COMMON_TABLE_H__

#include "storage/common/table_meta.h"

class DiskBufferPool;
class RecordFileHandler;
class OverflowFileHandler;
class ConditionFilter;
class DefaultConditionFilter;
struct Record;
struct RID;
struct OID;
class Index;
class IndexScanner;
class RecordDeleter;
class Trx;

class Table {
public:
  Table();
  ~Table();

  /**
   * 创建一个表
   * @param path 元数据保存的文件(完整路径)
   * @param name 表名
   * @param base_dir 表数据存放的路径
   * @param attribute_count 字段个数
   * @param attributes 字段
   */
  RC create(const char *path, const char *name, const char *base_dir, int attribute_count, const AttrInfo attributes[]);

  /**
   * 删除一个表
   */
  RC drop();

  /**
   * 打开一个表
   * @param meta_file 保存表元数据的文件完整路径
   * @param base_dir 表所在的文件夹，表记录数据文件、索引数据文件存放位置
   */
  RC open(const char *meta_file, const char *base_dir);
  
  RC insert_record(Trx *trx, int value_num, const Value *values);
  RC insert_records(Trx *trx, int tuple_num, const int value_nums[], const Value values[][MAX_NUM]);
  RC update_record(Trx *trx, const char *attribute_name, const Value *value, int condition_num, const Condition conditions[], int *updated_count);
  RC delete_record(Trx *trx, ConditionFilter *filter, int *deleted_count);

  RC get_record(const OID *oid, char *data);
  RC delete_record(const OID *oid);
  RC update_record(const char *data, int data_len, const OID *old_oid, OID *new_oid);
  
  RC scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, void (*record_reader)(const char *data, void *context));

  RC create_index(Trx *trx, const char *index_name, const char * const * attribute_names, int attribute_num, bool unique = false) ;

public:
  const char *name() const;

  const TableMeta &table_meta() const;

  RC sync();

public:
  RC commit_insert(Trx *trx, const RID &rid);
  RC commit_delete(Trx *trx, const RID &rid);
  RC rollback_insert(Trx *trx, const RID &rid);
  RC rollback_delete(Trx *trx, const RID &rid);

private:
  RC scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context));
  RC scan_record_by_index(Trx *trx, IndexScanner *scanner, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context));
  IndexScanner *find_index_for_scan(const ConditionFilter *filter);
  IndexScanner *find_index_for_scan(const DefaultConditionFilter &filter);
  IndexScanner *find_muti_index_for_scan(const ConditionFilter *filter);

  RC insert_record(Trx *trx, Record *record, int value_num, const Value *values);
  RC update_record(Trx *trx, Record *record);
  RC delete_record(Trx *trx, Record *record);

private:
  friend class RecordUpdater;
  friend class RecordDeleter;

  RC insert_entry_of_indexes(const char *record, const RID &rid);
  RC delete_entry_of_indexes(const char *record, const RID &rid, bool error_on_not_exists);
private:
  RC init_record_handler(const char *base_dir);
  RC make_record(int value_num, const Value *values, char * &record_out);
  RC insert_record(int value_num, const Value *values, char * record_in);  // 为了将overflow page的insert处理在事务启动后，额外封装一个函数
private:
  Index *find_index(const char *index_name) const;
  Index *find_index_by_field(const char *field_name) const;
  void find_index_contain_field(const char *field_name, std::vector<Index*> & indexes) const;

private:
  std::string             base_dir_;
  TableMeta               table_meta_;
  DiskBufferPool *        data_buffer_pool_; /// 数据文件关联的buffer pool
  std::string             file_name_;
  std::string             of_file_name_;
  RecordFileHandler *     record_handler_;   /// 记录操作
  OverflowFileHandler *   overflow_handler_; 
  std::vector<Index *>    indexes_;
};

#endif // __OBSERVER_STORAGE_COMMON_TABLE_H__