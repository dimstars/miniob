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
// Created by Wangyunlai on 2021/5/13.
//

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

#include "storage/common/table.h"
#include "storage/common/table_meta.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "common/lang/bitmap.h"
#include "storage/default/disk_buffer_pool.h"
#include "storage/common/record_manager.h"
#include "storage/common/condition_filter.h"
#include "storage/common/meta_util.h"
#include "storage/common/index.h"
#include "storage/common/bplus_tree_index.h"
#include "storage/trx/trx.h"

using namespace common;

Table::Table() : 
    data_buffer_pool_(nullptr),
    record_handler_(nullptr),
    overflow_handler_(nullptr)
    {
}

Table::~Table() {
  delete record_handler_;
  record_handler_ = nullptr;
  if(overflow_handler_) delete overflow_handler_;
  overflow_handler_ = nullptr;

  if (data_buffer_pool_ != nullptr) {
    if(!file_name_.empty()) {
      data_buffer_pool_->close_file(file_name_.c_str());
    }
    if(!of_file_name_.empty()) {
      data_buffer_pool_->close_file(of_file_name_.c_str());
    }
    data_buffer_pool_ = nullptr;
  }

  LOG_INFO("Table has been closed: %s", name());
}

RC Table::create(const char *path, const char *name, const char *base_dir, int attribute_count, const AttrInfo attributes[]) {

  if (nullptr == name || common::is_blank(name)) {
    LOG_WARN("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }

  if (attribute_count <= 0 || nullptr == attributes) {
    LOG_WARN("Invalid arguments. table_name=%s, attribute_count=%d, attributes=%p",
        name, attribute_count, attributes);
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;

  // 使用 table_name.table记录一个表的元数据
  // 判断表文件是否已经存在

  int fd = ::open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
  if (-1 == fd) {
    if (EEXIST == errno) {
      return RC::SCHEMA_TABLE_EXIST;
    }
    LOG_ERROR("Create table file failed. filename=%s, errmsg=%d:%s", 
       path, errno, strerror(errno));
    return RC::IOERR;
  }

  close(fd);

  // 创建文件
  if ((rc = table_meta_.init(name, attribute_count, attributes)) != RC::SUCCESS) {
    LOG_ERROR("Failed to init table meta. ret=%d", rc);
    return rc; // TODO delete table file
  }

  std::fstream fs;
  fs.open(path, std::ios_base::out | std::ios_base::binary);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", path, strerror(errno));
    return RC::IOERR;
  }

  // 记录元数据到文件中
  table_meta_.serialize(fs);
  fs.close();

  std::string data_file = std::string(base_dir) + "/" + name + TABLE_DATA_SUFFIX;
  data_buffer_pool_ = theGlobalDiskBufferPool();
  rc = data_buffer_pool_->create_file(data_file.c_str());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to create disk buffer pool of data file. file name=%s", data_file.c_str());
    return rc;
  }

  if(table_meta_.overflow_exist()) {
    std::string of_file = table_overflow_file(base_dir, name);
    rc = data_buffer_pool_->create_file(of_file.c_str());
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to create disk buffer pool of overflow file. file name=%s", of_file.c_str());
      return rc;
    }
  }

  rc = init_record_handler(base_dir);

  base_dir_ = base_dir;
  return rc;
}

RC Table::drop() {
  // 删除元数据文件
  std::string meta_file = base_dir_ + "/" + std::string(table_meta_.name()) + TABLE_META_SUFFIX;
  if (remove(meta_file.c_str()) != 0) {
    LOG_ERROR("Failed to remove meta file. file name = %s, errmsg = %s", meta_file.c_str(), strerror(errno));
    return RC::IOERR;
  }

  //删除索引文件
  const int index_num = table_meta_.index_num();
  for (int i = 0; i < index_num; i++) {
    const IndexMeta *index_meta = table_meta_.index(i);
    std::string index_file = index_data_file(base_dir_.c_str(), name(), index_meta->name());
    delete indexes_[i];
    if (remove(index_file.c_str()) != 0) {
      LOG_ERROR("Failed to remove index file. file name = %s, errmsg = %s", index_file.c_str(), strerror(errno));
      return RC::IOERR;
    }
  }


  // 删除数据文件
  std::string data_file = base_dir_ + "/" + std::string(table_meta_.name()) + TABLE_DATA_SUFFIX;
  if (remove(data_file.c_str()) != 0) {
    LOG_ERROR("Failed to remove data file. file name = %s, errmsg = %s", data_file.c_str(), strerror(errno));
    return RC::IOERR;
  }

  if(table_meta_.overflow_exist()) {
    // 删除overflow文件
    std::string of_file = base_dir_ + "/" + std::string(table_meta_.name()) + TABLE_OVERFLOW_SUFFIX;
    if (access(of_file.c_str(), F_OK) != -1 && remove(of_file.c_str()) != 0) {
      LOG_ERROR("Failed to remove overflow file. file name = %s, errmsg = %s", of_file.c_str(), strerror(errno));
      return RC::IOERR;
    }
  }

  return RC::SUCCESS;
}

RC Table::open(const char *meta_file, const char *base_dir) {
  // 加载元数据文件
  std::fstream fs;
  std::string meta_file_path = std::string(base_dir) + "/" + meta_file;
  fs.open(meta_file_path, std::ios_base::in | std::ios_base::binary);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open meta file for read. file name=%s, errmsg=%s", meta_file, strerror(errno));
    return RC::IOERR;
  }
  if (table_meta_.deserialize(fs) < 0) {
    LOG_ERROR("Failed to deserialize table meta. file name=%s", meta_file);
    return RC::GENERIC_ERROR;
  }
  fs.close();

  // 加载数据文件
  RC rc = init_record_handler(base_dir);

  base_dir_ = base_dir;

  const int index_num = table_meta_.index_num();
  for (int i = 0; i < index_num; i++) {
    const IndexMeta *index_meta = table_meta_.index(i);
    std::vector<const FieldMeta *> field_metas;
    for(int i = 0; i < index_meta->field_num(); ++i){
      const FieldMeta *field_meta = table_meta_.field(index_meta->field(i));
      if (field_meta == nullptr) {
        LOG_PANIC("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
                  name(), index_meta->name(), index_meta->field(i));
        return RC::GENERIC_ERROR;
      }
      field_metas.push_back(field_meta);
    }
    BplusTreeIndex *index = new BplusTreeIndex();
    std::string index_file = index_data_file(base_dir, name(), index_meta->name());
    bool unique = strstr(index_meta->extended_attr(),"unique") != nullptr;
    rc = index->open(index_file.c_str(), *index_meta, &field_metas, unique);
    if (rc != RC::SUCCESS) {
      delete index;
      LOG_ERROR("Failed to open index. table=%s, index=%s, file=%s, rc=%d:%s",
                name(), index_meta->name(), index_file.c_str(), rc, strrc(rc));
      return rc;
    }
    indexes_.push_back(index);
  }
  return rc;
}

RC Table::commit_insert(Trx *trx, const RID &rid) {
  Record record;
  RC rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return trx->commit_insert(this, record);
}

RC Table::rollback_insert(Trx *trx, const RID &rid) {

  Record record;
  RC rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  // remove all indexes
  rc = delete_entry_of_indexes(record.data, rid, false);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to delete indexes of record(rid=%d.%d) while rollback insert, rc=%d:%s",
              rid.page_num, rid.slot_num, rc, strrc(rc));
  } else {
    rc = record_handler_->delete_record(&rid);
  }
  return rc;
}

RC Table::insert_record(Trx *trx, Record *record, int value_num, const Value *values) {
  RC rc = RC::SUCCESS;

  if (trx != nullptr) {
    trx->init_trx_info(this, *record);
  }

  rc = insert_record(value_num, values, record->data);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert overflow entry failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
    return rc;
  }

  rc = record_handler_->insert_record(record->data, table_meta_.record_size(), &record->rid);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert record failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
    return rc;
  }

  if (trx != nullptr) {
    rc = trx->insert_record(this, record);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to log operation(insertion) to trx");

      RC rc2 = record_handler_->delete_record(&record->rid);
      if (rc2 != RC::SUCCESS) {
        LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                  name(), rc2, strrc(rc2));
      }
      return rc;
    }
  }

  rc = insert_entry_of_indexes(record->data, record->rid);
  if (rc != RC::SUCCESS) {
    RC rc2 = delete_entry_of_indexes(record->data, record->rid, true);
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC("Failed to rollback index data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
    rc2 = record_handler_->delete_record(&record->rid);
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
    return rc;
  }
  return rc;
}

RC Table::insert_record(Trx *trx, int value_num, const Value *values) {
  if (value_num <= 0 || nullptr == values ) {
    LOG_ERROR("Invalid argument. value num=%d, values=%p", value_num, values);
    return RC::INVALID_ARGUMENT;
  }

  char *record_data;
  RC rc = make_record(value_num, values, record_data);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to create a record. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  Record record;
  record.data = record_data;
  // record.valid = true;
  rc = insert_record(trx, &record, value_num, values);
  delete record_data;
  return rc;
}

RC Table::insert_records(Trx *trx, int tuple_num, const int value_nums[], const Value values[][MAX_NUM]) {
  for (int i = 0; i < tuple_num; i++) {
    RC rc = insert_record(trx, value_nums[i], values[i]);
    if (rc != RC::SUCCESS) {
      // TODO 在trx=nullptr时手动调用delete_record进行回退
      return rc;
    }
  }

  return RC::SUCCESS;
}

const char *Table::name() const {
  return table_meta_.name();
}

const TableMeta &Table::table_meta() const {
  return table_meta_;
}

RC Table::make_record(int value_num, const Value *values, char * &record_out) {
  // 检查字段类型数量是否一致
  if (value_num + table_meta_.sys_field_num() != table_meta_.field_num()) {
    return RC::SCHEMA_FIELD_MISSING;
  }

  const int normal_field_start_index = table_meta_.sys_field_num();
  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];
    if ((field->type() == INTS && value.type == FLOATS) 
          || (field->type() == INTS && value.type == FLOATS)
          || (field->nullable() && value.type == NULLS)
          || (field->type() == TEXTS && value.type == CHARS)) {
      // do nothing
    } else if(!field->nullable() && value.type == NULLS) {
      LOG_ERROR("Null not support. field name=%s", field->name());
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    } else if (field->type() != value.type) {
      LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
        field->name(), field->type(), value.type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    } else if(value.type == DATES && !check_date_legality((char*)value.data)) {
      LOG_ERROR("Illeagl value.");
      return RC::SCHEMA_FIELD_VALUE_ILLEGAL;
    }
  }

  // 复制所有字段的值
  int record_size = table_meta_.record_size();
  char *record = new char [record_size];

  Bitmap bitmap(record, RECORD_BITMAP_BITS);

  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];
    // TODO 为了不修改const参数，额外使用空间，后续可尝试优化
    if(field->type() == INTS && value.type == FLOATS) {
      int data = (int)*(float*)value.data;
      Value v;
      value_init_integer(&v, data);
      memcpy(record + field->offset(), v.data, field->len());
      value_destroy(&v);
    } else if(field->type() == FLOATS && value.type == INTS) {
      float data = (float)*(int*)value.data;
      Value v;
      value_init_float(&v, data);
      memcpy(record + field->offset(), v.data, field->len());
      value_destroy(&v);
    } else if(value.type == NULLS) {
      bitmap.set_bit(field->index());
    } else if(field->type() == TEXTS) {
      // 这里填充的值并没有什么作用
      memcpy(record + field->offset(), &(value.data), sizeof(char*));
    } else {
      memcpy(record + field->offset(), value.data, field->len());
    }
  }

  record_out = record;
  return RC::SUCCESS;
}

RC Table::insert_record(int value_num, const Value *values, char * record_in) {
  RC rc = RC::SUCCESS;

  const int normal_field_start_index = table_meta_.sys_field_num();
  Bitmap bitmap(record_in, RECORD_BITMAP_BITS);

  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];
    if(value.type == NULLS) continue;
    if(field->type() == TEXTS) {
      OID oid;
      // TODO 可能需要strlen+1
      rc = overflow_handler_->insert_record((char*)value.data, strlen((char*)value.data), &oid);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Insert text field data failed. field name=%s, rc=%d:%s", field->name(), rc, strrc(rc));
        return rc;
      }
      memcpy(record_in + field->offset(), &(oid), sizeof(oid));
    } 
  }

  return rc;
}

RC Table::init_record_handler(const char *base_dir) {
  std::string data_file = std::string(base_dir) + "/" + table_meta_.name() + TABLE_DATA_SUFFIX;
  if (nullptr == data_buffer_pool_) {
    data_buffer_pool_ = theGlobalDiskBufferPool();
  }

  RC rc = RC::SUCCESS;
  if(table_meta_.overflow_exist()) {
    std::string overflow_file = table_overflow_file(base_dir, table_meta_.name());

    overflow_handler_ = new OverflowFileHandler();
    rc = overflow_handler_->init(*data_buffer_pool_, overflow_file.c_str());
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to init overflow handler. rc=%d:%s", rc, strrc(rc));
      return rc;
    }
    of_file_name_ = overflow_file.c_str();
  }

  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open disk buffer pool. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  record_handler_ = new RecordFileHandler();
  rc = record_handler_->init(*data_buffer_pool_, data_file.c_str());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to init record handler. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  file_name_ = data_file;
  return rc;
}

/**
 * 为了不把Record暴露出去，封装一下
 */
class RecordReaderScanAdapter {
public:
  explicit RecordReaderScanAdapter(void (*record_reader)(const char *data, void *context), void *context)
      : record_reader_(record_reader), context_(context){
  }

  void consume(const Record *record) {
    record_reader_(record->data, context_);
  }
private:
  void (*record_reader_)(const char *, void *);
  void *context_;
};
static RC scan_record_reader_adapter(Record *record, void *context) {
  RecordReaderScanAdapter &adapter = *(RecordReaderScanAdapter *)context;
  adapter.consume(record);
  return RC::SUCCESS;
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, void (*record_reader)(const char *data, void *context)) {
  RecordReaderScanAdapter adapter(record_reader, context);
  return scan_record(trx, filter, limit, (void *)&adapter, scan_record_reader_adapter);
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context)) {
  if (nullptr == record_reader) {
    return RC::INVALID_ARGUMENT;
  }

  if (0 == limit) {
    return RC::SUCCESS;
  }

  if (limit < 0) {
    limit = INT_MAX;
  }

  IndexScanner *index_scanner = find_muti_index_for_scan(filter);

  if (index_scanner != nullptr) {
    return scan_record_by_index(trx, index_scanner, filter, limit, context, record_reader);
  }
  RC rc = RC::SUCCESS;
  RecordFileScanner scanner;
  rc = scanner.open_scan(*data_buffer_pool_, file_name_.c_str(), filter);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. file =%s. rc=%d:%s", file_name_.c_str(), rc, strrc(rc));
    return rc;
  }

  int record_count = 0;
  Record record;
  rc = scanner.get_first_record(&record);
  for ( ; RC::SUCCESS == rc && record_count < limit; rc = scanner.get_next_record(&record)) {
    if (trx == nullptr || trx->is_visible(this, &record)) {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS) {
        break;
      }
      record_count++;
    }
  }

  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_ERROR("failed to scan record. file =%s, rc=%d:%s", file_name_.c_str(), rc, strrc(rc));
  }
  scanner.close_scan();
  return rc;
}

RC Table::scan_record_by_index(Trx *trx, IndexScanner *scanner, ConditionFilter *filter, int limit, void *context,
                               RC (*record_reader)(Record *, void *)) {
  RC rc = RC::SUCCESS;
  RID rid;
  Record record;
  int record_count = 0;
  while (record_count < limit) {
    rc = scanner->next_entry(&rid);
    if (rc != RC::SUCCESS) {
      if (RC::RECORD_EOF == rc) {
        rc = RC::SUCCESS;
        break;
      }
      LOG_ERROR("Failed to scan table by index. rc=%d:%s", rc, strrc(rc));
      break;
    }

    rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to fetch record of rid=%d:%d, rc=%d:%s", rid.page_num, rid.slot_num, rc, strrc(rc));
      break;
    }
    
    if ((trx == nullptr || trx->is_visible(this, &record)) && (filter == nullptr || filter->filter(record))) {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS) {
        LOG_TRACE("Record reader break the table scanning. rc=%d:%s", rc, strrc(rc));
        break;
      }
    }

    record_count++;
  }

  scanner->destroy();
  return rc;
}

class IndexInserter {
public:
  explicit IndexInserter(Index *index) : index_(index) {
  }

  RC insert_index(const Record *record) {
    return index_->insert_entry(record->data, &record->rid);
  }
private:
  Index * index_;
};

static RC insert_index_record_reader_adapter(Record *record, void *context) {
  IndexInserter &inserter = *(IndexInserter *)context;
  return inserter.insert_index(record);
}

RC Table::create_index(Trx *trx, const char *index_name, const char * const *attribute_names, int attribute_num, bool unique) {
  if (index_name == nullptr || common::is_blank(index_name) ||
      attribute_num == 0 || attribute_names == nullptr) {
    return RC::INVALID_ARGUMENT;
  }
  for(int i = 0; i < attribute_num; ++i){
    if(attribute_names[i] == nullptr || common::is_blank(attribute_names[i])){
      return RC::INVALID_ARGUMENT;
    }
  }
  if (table_meta_.index(index_name) != nullptr) {
    return RC::SCHEMA_INDEX_EXIST;
  }
  bool exist = table_meta_.check_index_existance(attribute_names,attribute_num);
  if(exist){
    return RC::SCHEMA_INDEX_EXIST;
  }
  
  std::vector<const FieldMeta *>field_metas;
  const FieldMeta *field_meta;
  for(int i = 0; i < attribute_num; ++i){
    field_meta = table_meta_.field(attribute_names[i]);
    if(!field_meta){
      return RC::SCHEMA_FIELD_MISSING;
    }
    field_metas.push_back(field_meta);
  }
  IndexMeta new_index_meta;
  RC rc = new_index_meta.init(index_name, field_metas, unique, field_metas.size());
  if (rc != RC::SUCCESS) {
    return rc;
  }
  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create(index_file.c_str(), new_index_meta, &field_metas, unique);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }
  // 遍历当前的所有数据，插入这个索引
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS) {
    // TODO rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
     remove(index_file.c_str());
    return rc;
  }
  indexes_.push_back(index);
  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    remove(index_file.c_str());
    return rc;;
  }
  // 创建元数据临时文件
  std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  std::fstream fs;
  fs.open(tmp_file, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    remove(index_file.c_str());
    return RC::IOERR; // TODO 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    remove(index_file.c_str());
    return RC::IOERR;
  }
  fs.close();

  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). " \
              "system error=%d:%s", tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

  return rc;
}

class RecordUpdater {
public:
  RecordUpdater(Table &table, Trx *trx, const char *attribute_name, const Value *value) : 
    table_(table), trx_(trx), attribute_name_(attribute_name){
      table_.find_index_contain_field(attribute_name,index_);
      const FieldMeta *field = table_.table_meta_.field(attribute_name_);
      if(field->type() == INTS && value->type == FLOATS) {
        int data = (int)*(float*)value->data;
        value_init_integer(&value_, data);
      } else if(field->type() == FLOATS && value->type == INTS) {
        float data = (float)*(int*)value->data;
        value_init_float(&value_, data);
      } else if(value->type == NULLS) {
        value_init_null(&value_);
      } else if(field->type() == TEXTS) {
        value_.type = value->type;
        value_.data = (char*)malloc(strlen((char*)value->data)+1);
        memcpy(value_.data,value->data,strlen((char*)value->data)+1);
      } else {
        value_.type = value->type;
        value_.data = (char*)malloc(field->len());
        memcpy(value_.data,value->data,field->len());
      }
  }
  ~RecordUpdater(){
    value_destroy(&value_);
  }

  RC collect_records(Record *record){
    const FieldMeta *field = table_.table_meta_.field(attribute_name_);
    Bitmap bitmap(record->data, RECORD_BITMAP_BITS);
    if((value_.type == NULLS && !bitmap.get_bit(field->index())) 
        || bitmap.get_bit(field->index())
        || 0 != strncmp((char*)value_.data, record->data + field->offset(), field->len())) {
      rids_.push_back(record->rid);
    }
      
    return RC::SUCCESS;
  }

  RC update_record() {
    RC rc = RC::SUCCESS;
    const FieldMeta *field = table_.table_meta_.field(attribute_name_);
    Record record;
    char *record_new = (char*)malloc(sizeof(char) * table_.table_meta().record_size());
    //更新索引
    for(int i = 0; i < rids_.size(); ++i){
      rc = table_.record_handler_->get_record(&rids_[i],&record);
      if(RC::SUCCESS != rc){
        return rc;
      }
      memcpy(record_new, record.data, table_.table_meta().record_size());
      Bitmap bitmap(record_new, RECORD_BITMAP_BITS);
      if(value_.type == NULLS) {
        bitmap.set_bit(field->index());
        // record内的原数据没有清除
      } else {
        memcpy(record_new + field->offset(), (char*)value_.data, field->len());
        if(bitmap.get_bit(field->index())) bitmap.clear_bit(field->index());
      }
      for(int j = 0; j < index_.size(); ++j){
        rc = index_[j]->insert_entry(record_new, &record.rid);
        if(RC::SUCCESS != rc){
          break;
        }
        index_[j]->delete_entry(record.data,&record.rid);
      }
      if(RC::SUCCESS != rc)
        break;
    }
    if(RC::SUCCESS != rc){
      //回滚
      RC rollback_rc;
      for(int i = 0; i < rids_.size(); ++i){
        rollback_rc = table_.record_handler_->get_record(&rids_[i],&record);
        if(RC::SUCCESS != rollback_rc){
          LOG_ERROR("fail to rollback update operation");
          free(record_new);
          return rc;
        }
        memcpy(record_new, record.data, table_.table_meta().record_size());
        Bitmap bitmap(record_new, RECORD_BITMAP_BITS);
        if(value_.type == NULLS) {
          bitmap.set_bit(field->index());
          // record内的原数据没有清除
        } else {
          memcpy(record_new + field->offset(), (char*)value_.data, field->len());
          if(bitmap.get_bit(field->index())) bitmap.clear_bit(field->index());
        }
        for(int j = 0; j < index_.size(); ++j){
          rollback_rc = index_[j]->delete_entry(record_new,&record.rid);
          if(RC::SUCCESS != rollback_rc){
             free(record_new);
            return rc;
          }
          index_[j]->insert_entry(record.data, &record.rid);
        }
      }
      free(record_new);
      return rc;
    }
    for(int i = 0; i < rids_.size(); ++i){
      //update data
      rc = table_.record_handler_->get_record(&rids_[i],&record);
      Bitmap bitmap(record.data, RECORD_BITMAP_BITS);
      if(value_.type == NULLS) {
        if(!bitmap.get_bit(field->index()) && field->type() == TEXTS) {
          OID oid = *(OID*)(record.data + field->offset());
          table_.delete_record(&oid);
        }
        bitmap.set_bit(field->index());
        // record内的原数据value没有清除
      } else {
        if(!bitmap.get_bit(field->index())) {
          if(field->type() == TEXTS) {
            OID old_oid = *(OID*)(record.data + field->offset());
            OID new_oid;
            table_.update_record((char*)value_.data, strlen((char*)value_.data), &old_oid, &new_oid);
            memcpy(record.data + field->offset(), &new_oid, field->len());
          } else {
            memcpy(record.data + field->offset(), (char*)value_.data, field->len());
          }
        } else {
          if(field->type() == TEXTS) {
            OID new_oid;
            table_.update_record((char*)value_.data, strlen((char*)value_.data), nullptr, &new_oid);
            memcpy(record.data + field->offset(), &new_oid, field->len());
          } else {
            memcpy(record.data + field->offset(), (char*)value_.data, field->len());
          }
          bitmap.clear_bit(field->index());
        }
      }
      rc = table_.update_record(trx_, &record);
      if (rc == RC::SUCCESS) {
        updated_count_++;
      }
    }
    free(record_new);
    record_new = nullptr;
    return rc;
  }

  int updated_count() const {
    return updated_count_;
  }

private:
  Table & table_;
  Trx *trx_;
  const char *attribute_name_;
  Value value_;
  int updated_count_ = 0;
  std::vector<Index *>index_;
  std::vector<RID> rids_;
};

static RC record_reader_update_adapter(Record *record, void *context) {
  RecordUpdater &record_updater = *(RecordUpdater *)context;
  //return record_updater.update_record(record);
  return record_updater.collect_records(record);
}

RC Table::update_record(Trx *trx, const char *attribute_name, const Value *value, int condition_num, const Condition conditions[], int *updated_count) {
  RecordUpdater updater(*this, trx, attribute_name, value);

  CompositeConditionFilter condition_filter;
  RC rc = condition_filter.init(*this, conditions, condition_num);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  scan_record(trx, &condition_filter, -1, &updater, record_reader_update_adapter);
  rc = updater.update_record();
  if (updated_count != nullptr) {
    *updated_count = updater.updated_count();
  }
  return rc;
}

RC Table::update_record(Trx *trx, Record *record) {
  RC rc = RC::SUCCESS;
  // TODO: if (trx != nullptr)
  rc = record_handler_->update_record(record);
  return rc;
}

class RecordDeleter {
public:
  RecordDeleter(Table &table, Trx *trx) : table_(table), trx_(trx) {
  }

  RC delete_record(Record *record) {
    RC rc = RC::SUCCESS;
    rc = table_.delete_record(trx_, record);
    if (rc == RC::SUCCESS) {
      deleted_count_++;
    }
    return rc;
  }

  int deleted_count() const {
    return deleted_count_;
  }

private:
  Table & table_;
  Trx *trx_;
  int deleted_count_ = 0;
};

static RC record_reader_delete_adapter(Record *record, void *context) {
  RecordDeleter &record_deleter = *(RecordDeleter *)context;
  return record_deleter.delete_record(record);
}

RC Table::delete_record(Trx *trx, ConditionFilter *filter, int *deleted_count) {
  RecordDeleter deleter(*this, trx);
  RC rc = scan_record(trx, filter, -1, &deleter, record_reader_delete_adapter);
  if (deleted_count != nullptr) {
    *deleted_count = deleter.deleted_count();
  }
  return rc;
}

RC Table::delete_record(Trx *trx, Record *record) {
  RC rc = RC::SUCCESS;
  if (trx != nullptr) {
    rc = trx->delete_record(this, record);
  } else {
    rc = delete_entry_of_indexes(record->data, record->rid, false);// TODO 重复代码 refer to commit_delete
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to delete indexes of record (rid=%d.%d). rc=%d:%s",
                record->rid.page_num, record->rid.slot_num, rc, strrc(rc));
    } else {
      Bitmap bitmap(record->data, RECORD_BITMAP_BITS);
      for(int i = 0; i < table_meta_.field_num(); i++) {
        if(table_meta_.field(i)->type() == TEXTS && !bitmap.get_bit(i)) {
          OID oid = *(OID*)(record->data+table_meta_.field(i)->offset());
          rc = overflow_handler_->delete_record(&oid);
        } 
      }
      rc = record_handler_->delete_record(&record->rid);
    }
  }
  return rc;
}

RC Table::get_record(const OID *oid, char *data) {
  if(overflow_handler_ == nullptr) {
    LOG_ERROR("overflow file handler is nullptr");
    return RC::GENERIC_ERROR;
  }
  overflow_handler_->get_record(oid, data);
  return RC::SUCCESS;
}

RC Table::delete_record(const OID *oid) {
  if(overflow_handler_ == nullptr) {
    LOG_ERROR("overflow file handler is nullptr");
    return RC::GENERIC_ERROR;
  }
  overflow_handler_->delete_record(oid);
  return RC::SUCCESS;
}

RC Table::update_record(const char *data, int data_len, const OID *old_oid, OID *new_oid) {
  if(overflow_handler_ == nullptr) {
    LOG_ERROR("overflow file handler is nullptr");
    return RC::GENERIC_ERROR;
  }

  if(old_oid) overflow_handler_->update_record(data, data_len, old_oid, new_oid);
  else overflow_handler_->insert_record(data, data_len, new_oid);

  return RC::SUCCESS;
}

RC Table::commit_delete(Trx *trx, const RID &rid) {
  RC rc = RC::SUCCESS;
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  rc = delete_entry_of_indexes(record.data, record.rid, false);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
              rid.page_num, rid.slot_num, rc, strrc(rc));// TODO panic?
  }

  Bitmap bitmap(record.data, RECORD_BITMAP_BITS);
  for(int i = 0; i < table_meta_.field_num(); i++) {
    if(table_meta_.field(i)->type() == TEXTS && !bitmap.get_bit(i)) {
      LOG_WARN("delete overflow page");
      OID oid = *(OID*)(record.data+table_meta_.field(i)->offset());
      rc = overflow_handler_->delete_record(&oid);
    } 
  }

  rc = record_handler_->delete_record(&rid);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return rc;
}

RC Table::rollback_delete(Trx *trx, const RID &rid) {
  RC rc = RC::SUCCESS;
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return trx->rollback_delete(this, record); // TODO update record in place
}

RC Table::insert_entry_of_indexes(const char *record, const RID &rid) {
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->insert_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      break;
    }
  }
  return rc;
}

RC Table::delete_entry_of_indexes(const char *record, const RID &rid, bool error_on_not_exists) {
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->delete_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      if (rc != RC::RECORD_INVALID_KEY || !error_on_not_exists) {
        break;
      }
    }
  }
  return rc;
}

void Table::find_index_contain_field(const char *field_name, std::vector<Index*> & indexes) const {
  for (Index *index: indexes_) {
    int num = index->index_meta().field_num();
    for(int i = 0; i < num; ++ i){
      if (0 == strcmp(index->index_meta().field(i), field_name)) {
        indexes.push_back(index);
        break;
      }
    }
  }
}

Index *Table::find_index_by_field(const char *field_name) const {
  Index * idx = nullptr;
  for (Index *index: indexes_) {
    if (0 == strcmp(index->index_meta().field(0), field_name)) {
      if(!idx)
        idx = index;
      else if(index->index_meta().field_num() < idx->index_meta().field_num()){
        idx = index;
      }
    }
  }
  return idx;
}

Index *Table::find_index(const char *index_name) const {
  for (Index *index: indexes_) {
    if (0 == strcmp(index->index_meta().name(), index_name)) {
      return index;
    }
  }
  return nullptr;
}

IndexScanner *Table::find_muti_index_for_scan(const ConditionFilter *continditionfilter){
  std::vector<const char *> prefix_meta_names;//equal_to
  std::vector<const char *> prefix_value_cond_descs;
  std::vector<AttrType> prefix_value_types;
  std::vector<const char *> last_meta_names; //greater_than greater_equal
  std::vector<const char *> last_value_cond_descs;
  std::vector<AttrType> last_value_types;
  std::vector<CompOp> last_ops;
  std::vector<const char *> value_cond_descs;
  std::vector<AttrType> value_types;
  const CompositeConditionFilter *composite_condition_filter = dynamic_cast<const CompositeConditionFilter *>(continditionfilter);
  if (composite_condition_filter == nullptr) {
    return nullptr;
  }
  CompOp op;
  std::vector<DefaultConditionFilter> filters;
  for(int i = 0; i < composite_condition_filter->filter_num(); ++i){
    const DefaultConditionFilter * f = dynamic_cast<const DefaultConditionFilter *>(&composite_condition_filter->filter(i));
    if(f == nullptr){
      continue;
    }
    op = f->comp_op();
    if(op == IS_NULL) op = EQUAL_TO;
    else if(op == IS_NOT_NULL) return nullptr;
    if(op == NO_OP || op == LESS_EQUAL ||op == LESS_THAN){
      continue;
    }
    filters.push_back(*f);
    const DefaultConditionFilter & filter = filters.back();
    const ConDesc *field_cond_desc = nullptr;
    const ConDesc *value_cond_desc = nullptr;
    AttrType value_type = UNDEFINED;
    if (filter.left().is_attr && !filter.right().is_attr) {
      field_cond_desc = &filter.left();
      value_cond_desc = &filter.right();
      value_type = filter.right_type();
    } else if (filter.right().is_attr && !filter.left().is_attr) {
      field_cond_desc = &filter.right();
      value_cond_desc = &filter.left();
      value_type = filter.left_type();
    }
    if (field_cond_desc == nullptr || value_cond_desc == nullptr) {
      return nullptr;
    }
    const FieldMeta *field_meta = table_meta_.find_field_by_offset(field_cond_desc->attr_offset);
    if (nullptr == field_meta) {
      LOG_PANIC("Cannot find field by offset %d. table=%d, condition filter=%s",
                field_cond_desc->attr_offset, name(), ""); // TODO condition filter to string
      return nullptr;
    }
    if(op == EQUAL_TO){
      prefix_meta_names.push_back(field_meta->name());
      prefix_value_cond_descs.push_back((const char*)(value_cond_desc->value));
      prefix_value_types.push_back(value_type);
    }else if(op == GREAT_THAN || op == GREAT_EQUAL){
      last_meta_names.push_back(field_meta->name());
      last_value_cond_descs.push_back((const char*)(value_cond_desc->value));
      last_value_types.push_back(value_type);
      last_ops.push_back(op);
    }
  }

  const IndexMeta *index_meta = table_meta_.find_index_by_field(&prefix_meta_names,&last_meta_names);
  if (nullptr == index_meta) {
    return find_index_for_scan(continditionfilter);
  }

  Index *index = find_index(index_meta->name());
  if (nullptr == index) {
    return nullptr;
  }
  LOG_INFO("use index : %s", index->index_meta().name());
  op = EQUAL_TO;
  for(int i = 0 ; i < index_meta->field_num(); ++i){
    bool flag = false;
    for(int p = 0; p < prefix_meta_names.size(); ++p){
      if(0 == strcmp(prefix_meta_names[p],index_meta->field(i))){
        value_cond_descs.push_back(prefix_value_cond_descs[p]);
        value_types.push_back(prefix_value_types[p]);
        flag = true;
        break;
      }
    }
    if(!flag){
      for(int l = 0; l < last_meta_names.size(); ++l){
        if(0 == strcmp(last_meta_names[l],index_meta->field(i))){
          value_cond_descs.push_back(last_value_cond_descs[l]);
          value_types.push_back(last_value_types[l]);
          op = last_ops[l];
          break;
        }
      }
      break;
    }
  }
  return index->create_scanner(op, &value_cond_descs, &value_types);
}

IndexScanner *Table::find_index_for_scan(const DefaultConditionFilter &filter) {
  const ConDesc *field_cond_desc = nullptr;
  const ConDesc *value_cond_desc = nullptr;
  AttrType value_type = UNDEFINED;
  CompOp op = NO_OP;
  if (filter.left().is_attr && !filter.right().is_attr) {
    field_cond_desc = &filter.left();
    value_cond_desc = &filter.right();
    value_type = filter.right_type();
    op = filter.comp_op();
    if(op == IS_NULL) op = EQUAL_TO;
    else if(op == IS_NOT_NULL) return nullptr;
  } else if (filter.right().is_attr && !filter.left().is_attr) {
    field_cond_desc = &filter.right();
    value_cond_desc = &filter.left();
    value_type = filter.left_type();
    switch (filter.comp_op())
    {
      case LESS_EQUAL: 
        op =GREAT_EQUAL;
      break;
      case LESS_THAN:  //"<"			3
        op = GREAT_THAN;
      break;
      case GREAT_EQUAL: //">="			4
        op = LESS_EQUAL;
      break;
      case GREAT_THAN: //">"           5
        op = LESS_THAN;
      break;
      case NO_OP:
      case EQUAL_TO:
      case NOT_EQUAL:
        op = filter.comp_op();
      default:
        return nullptr;
      break;
    }
  }
  if (field_cond_desc == nullptr || value_cond_desc == nullptr) {
    return nullptr;
  }

  const FieldMeta *field_meta = table_meta_.find_field_by_offset(field_cond_desc->attr_offset);
  if (nullptr == field_meta) {
    LOG_PANIC("Cannot find field by offset %d. table=%d, condition filter=%s",
              field_cond_desc->attr_offset, name(), ""); // TODO condition filter to string
    return nullptr;
  }

  const IndexMeta *index_meta = table_meta_.find_index_by_field(field_meta->name());
  if (nullptr == index_meta) {
    return nullptr;
  }

  Index *index = find_index(index_meta->name());
  if (nullptr == index) {
    return nullptr;
  }
  LOG_INFO("use index : %s", index->index_meta().name());
  std::vector<const char*> values;
  values.push_back((const char *)(value_cond_desc->value));
  std::vector<AttrType> types;
  types.push_back(value_type);
  return index->create_scanner(op, &values, &types);
}

IndexScanner *Table::find_index_for_scan(const ConditionFilter *filter) {
  if (nullptr == filter) {
    return nullptr;
  }

  // TODO remove dynamic_cast
  const DefaultConditionFilter *default_condition_filter = dynamic_cast<const DefaultConditionFilter *>(filter);
  if (default_condition_filter != nullptr) {
    return find_index_for_scan(*default_condition_filter);
  }

  const CompositeConditionFilter *composite_condition_filter = dynamic_cast<const CompositeConditionFilter *>(filter);
  if (composite_condition_filter != nullptr) {
    int filter_num = composite_condition_filter->filter_num();
    for (int i = 0; i < filter_num; i++) {
      IndexScanner *scanner= find_index_for_scan(&composite_condition_filter->filter(i));
      if (scanner != nullptr) {
        return scanner; // TODO 可以找到一个最优的，比如比较符号是=
      }
    }
  }
  return nullptr;
}

RC Table::sync() {
  RC rc = data_buffer_pool_->flush_all_pages(file_name_.c_str());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to flush table's data pages. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }

  for (Index *index: indexes_) {
    rc = index->sync();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to flush index's pages. table=%s, index=%s, rc=%d:%s",
                name(), index->index_meta().name(), rc, strrc(rc));
      return rc;
    }
  }
  LOG_INFO("Sync table over. table=%s", name());
  return rc;
}
