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
#include "storage/common/record_manager.h"
#include "rc.h"
#include "common/log/log.h"
#include "common/lang/bitmap.h"
#include "condition_filter.h"

using namespace common;

// 除第一页外的普通页
// 包含pagenum+pageheader+bitmap+ records
struct PageHeader {
  int record_num;  // 当前页面记录的个数
  int record_capacity; // 最大记录个数
  int record_real_size; // 每条记录的实际大小
  int record_size; // 每条记录占用实际空间大小(可能对齐)
  int first_record_offset; // 第一条记录的偏移量
};

// overflow file page header
struct OFPageHeader {
  PageNum next_page_num;  // 如果next_page_num = 0表示这是text的最后一页
};

int align8(int size) {
  return size / 8 * 8 + ((size % 8 == 0) ? 0 : 8);
}

int page_fix_size() {
  return sizeof(PageHeader::record_num)
      + sizeof(PageHeader::record_capacity)
      + sizeof(PageHeader::record_real_size)
      + sizeof(PageHeader::record_size)
      + sizeof(PageHeader::first_record_offset);
}

int page_record_capacity(int page_size, int record_size) {
  // (record_capacity * record_size) + record_capacity/8 + 1 <= (page_size - fix_size)
  // ==> record_capacity = ((page_size - fix_size) - 1) / (record_size + 0.125)
  return (int)((page_size - page_fix_size() - 1) / (record_size + 0.125));
}

int page_bitmap_size(int record_capacity) {
  return record_capacity / 8 + ((record_capacity % 8 == 0) ? 0 : 1);
}

int page_header_size(int record_capacity) {
  const int bitmap_size = page_bitmap_size(record_capacity);
  return align8(page_fix_size() + bitmap_size);
}
////////////////////////////////////////////////////////////////////////////////
RecordPageHandler::RecordPageHandler() : 
    disk_buffer_pool_(nullptr),
    file_id_(-1),
    page_header_(nullptr),
    bitmap_(nullptr) {
}

RecordPageHandler::~RecordPageHandler() {
  deinit();
}

RC RecordPageHandler::init(DiskBufferPool &buffer_pool, int file_id, PageNum page_num) {
  if (disk_buffer_pool_ != nullptr) {
    return RC::RECORD_OPENNED;
  }

  RC ret = RC::SUCCESS;
  if ((ret = buffer_pool.get_this_page(file_id, page_num, &page_handle_)) != RC::SUCCESS) {
    LOG_ERROR("Failed to get page handle from disk buffer pool. ret=%d:%s", ret, strrc(ret));
    return ret;
  }

  char *data;
  ret = buffer_pool.get_data(&page_handle_, &data);
  if (ret != RC::SUCCESS) {
    LOG_ERROR("Failed to get page data. ret=%d:%s", ret, strrc(ret));
    return ret;
  }

  disk_buffer_pool_ = &buffer_pool;
  file_id_ = file_id;

  page_header_ = (PageHeader*)(data);
  bitmap_ = data + page_fix_size();
  return ret;
}

RC RecordPageHandler::init_empty_page(DiskBufferPool &buffer_pool, int file_id, PageNum page_num, int record_size) {
  RC ret = init(buffer_pool, file_id, page_num);
  if (ret != RC::SUCCESS) {
    return ret;
  }

  int page_size = sizeof(page_handle_.frame->page.data);
  int record_phy_size = align8(record_size);
  page_header_->record_num = 0;
  page_header_->record_capacity = page_record_capacity(page_size, record_phy_size);
  page_header_->record_real_size = record_size;
  page_header_->record_size = record_phy_size;
  page_header_->first_record_offset = page_header_size(page_header_->record_capacity);
  bitmap_ = page_handle_.frame->page.data + page_fix_size();

  memset(bitmap_, 0, page_bitmap_size(page_header_->record_capacity));
  disk_buffer_pool_->mark_dirty(&page_handle_);
  return RC::SUCCESS;
}

RC RecordPageHandler::deinit() {
  if (disk_buffer_pool_ != nullptr) {
    disk_buffer_pool_->unpin_page(&page_handle_);
    disk_buffer_pool_ = nullptr;
  }
  if (page_header_ != nullptr) {
    page_header_ = nullptr;
  }
  return RC::SUCCESS;
}

RC RecordPageHandler::insert_record(const char *data, RID *rid) {

  if (page_header_->record_num == page_header_->record_capacity) {
    return RC::RECORD_NOMEM;
  }

  // 找到空闲位置
  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  int index = bitmap.next_unsetted_bit(0);
  bitmap.set_bit(index);
  page_header_->record_num++;

  // assert index < page_header_->record_capacity
  char *record_data = page_handle_.frame->page.data +
      page_header_->first_record_offset + (index * page_header_->record_size);
  memcpy(record_data, data, page_header_->record_real_size);

  disk_buffer_pool_->mark_dirty(&page_handle_);

  if (rid) {
    rid->page_num = get_page_num();
    rid->slot_num = index;
  }

  LOG_TRACE("Insert record. rid page_num=%d, slot num=%d", get_page_num(), index);
  return RC::SUCCESS;
}

RC RecordPageHandler::update_record(const Record *rec) {
  RC ret = RC::SUCCESS;

  if (rec->rid.slot_num >= page_header_->record_capacity) {
    return RC::INVALID_ARGUMENT; // TODO 调整错误码
  }

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (!bitmap.get_bit(rec->rid.slot_num)) {
    ret = RC::RECORD_RECORD_NOT_EXIST;
  } else {
    char *record_data = page_handle_.frame->page.data +
        page_header_->first_record_offset + (rec->rid.slot_num * page_header_->record_size);
    memcpy(record_data, rec->data, page_header_->record_real_size);
    disk_buffer_pool_->mark_dirty(&page_handle_);
  }

  LOG_TRACE("Update record. page num=%d,slot=%d", rec->rid.page_num, rec->rid.slot_num);
  return ret;
}

RC RecordPageHandler::delete_record(const RID *rid) {
  RC ret = RC::SUCCESS;

  if (rid->slot_num >= page_header_->record_capacity) {
    return RC::INVALID_ARGUMENT; // TODO 调整错误码
  }

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (bitmap.get_bit(rid->slot_num)) {
    bitmap.clear_bit(rid->slot_num);
    page_header_->record_num--;
    disk_buffer_pool_->mark_dirty(&page_handle_);

    if (page_header_->record_num == 0) {
      DiskBufferPool *disk_buffer_pool = disk_buffer_pool_;
      int file_id = file_id_;
      PageNum page_num = get_page_num();
      if(page_num == 0) return ret;
      deinit();
      disk_buffer_pool->dispose_page(file_id, page_num);
    }
  } else {
    ret = RC::RECORD_RECORD_NOT_EXIST;
  }
  return ret;
}

RC RecordPageHandler::get_record(const RID *rid, Record *rec) {
  if (rid->slot_num >= page_header_->record_capacity) {
    return RC::RECORD_INVALIDRID;
  }

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (!bitmap.get_bit(rid->slot_num)) {
    return RC::RECORD_RECORD_NOT_EXIST;
  }

  char *data = page_handle_.frame->page.data +
      page_header_->first_record_offset + (page_header_->record_size * rid->slot_num);

  // rec->valid = true;
  rec->rid = *rid;
  rec->data = data;
  return RC::SUCCESS;
}

RC RecordPageHandler::get_first_record(Record *rec) {
  rec->rid.slot_num = -1;
  return get_next_record(rec);
}

RC RecordPageHandler::get_next_record(Record *rec) {
  if (rec->rid.slot_num >= page_header_->record_capacity - 1) {
    return RC::RECORD_EOF;
  }

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  int index = bitmap.next_setted_bit(rec->rid.slot_num + 1);

  if (index < 0) {
    return RC::RECORD_EOF;
  }

  rec->rid.page_num = get_page_num();
  rec->rid.slot_num = index;
  // rec->valid = true;

  char *record_data = page_handle_.frame->page.data +
      page_header_->first_record_offset + (index * page_header_->record_size);
  rec->data = record_data;
  return RC::SUCCESS;
}

PageNum RecordPageHandler::get_page_num() const {
  if (nullptr == page_header_) {
    return (PageNum)(-1);
  }
  return page_handle_.frame->page.page_num;
}

bool RecordPageHandler::is_full() const {
  return page_header_->record_num >= page_header_->record_capacity;
}

////////////////////////////////////////////////////////////////////////////////
OverflowPageHandler::OverflowPageHandler() : 
    disk_buffer_pool_(nullptr),
    file_id_(-1),
    page_header_(nullptr) {
}

OverflowPageHandler::~OverflowPageHandler() {
  deinit();
}

RC OverflowPageHandler::init(DiskBufferPool &buffer_pool, int file_id, PageNum page_num) {
  if (disk_buffer_pool_ != nullptr) {
    return RC::RECORD_OPENNED;
  }

  RC ret = RC::SUCCESS;
  if ((ret = buffer_pool.get_this_page(file_id, page_num, &page_handle_)) != RC::SUCCESS) {
    LOG_ERROR("Failed to get page handle from disk buffer pool. ret=%d:%s", ret, strrc(ret));
    return ret;
  }

  char *data;
  ret = buffer_pool.get_data(&page_handle_, &data);
  if (ret != RC::SUCCESS) {
    LOG_ERROR("Failed to get page data. ret=%d:%s", ret, strrc(ret));
    return ret;
  }

  disk_buffer_pool_ = &buffer_pool;
  file_id_ = file_id;

  page_header_ = (OFPageHeader*)(data);
  
  // 初始化就需要设置为dirty，因为page初始化就需要写入数据
  disk_buffer_pool_->mark_dirty(&page_handle_);

  return ret;
}

RC OverflowPageHandler::deinit() {
  if (disk_buffer_pool_ != nullptr) {
    disk_buffer_pool_->unpin_page(&page_handle_);
    disk_buffer_pool_ = nullptr;
  }
  if (page_header_ != nullptr) {
    page_header_ = nullptr;
  }
  return RC::SUCCESS;
}

RC OverflowPageHandler::insert_record(const char *data, int data_len, PageNum next_page_num) {
  page_header_->next_page_num = next_page_num;

  // assert index < page_header_->record_capacity
  char *record_data = page_handle_.frame->page.data + sizeof(page_header_);
  memcpy(record_data, data, data_len);

  disk_buffer_pool_->mark_dirty(&page_handle_);

  LOG_TRACE("Insert text. rid page_num=%d", get_page_num());
  return RC::SUCCESS;
}

RC OverflowPageHandler::update_record() {
  RC ret = RC::SUCCESS;

  return ret;
}

RC OverflowPageHandler::delete_record() {
  RC ret = RC::SUCCESS;

  DiskBufferPool *disk_buffer_pool = disk_buffer_pool_;
  int file_id = file_id_;
  PageNum page_num = get_page_num();
  if(page_num == 0) return ret;
  deinit();
  disk_buffer_pool->dispose_page(file_id, page_num);

  return ret;
}

RC OverflowPageHandler::get_record(char * &data) {
  char *page_data = page_handle_.frame->page.data + sizeof(page_header_);

  data = page_data;

  return RC::SUCCESS;
}

PageNum OverflowPageHandler::get_page_num() const {
  if (nullptr == page_header_) {
    return (PageNum)(-1);
  }
  return page_handle_.frame->page.page_num;
}

PageNum OverflowPageHandler::get_next_page_num() const {
  if (nullptr == page_header_) {
    return (PageNum)(-1);
  }
  return page_header_->next_page_num;
}
////////////////////////////////////////////////////////////////////////////////
RecordFileHandler::RecordFileHandler() :
    disk_buffer_pool_(nullptr),
    file_id_(-1) {
}

RC RecordFileHandler::init(DiskBufferPool &buffer_pool, int file_id) {

  RC ret = RC::SUCCESS;

  if (disk_buffer_pool_ != nullptr) {
    return RC::RECORD_OPENNED;
  }

  disk_buffer_pool_ = &buffer_pool;
  file_id_ = file_id;

  return ret;
}

void RecordFileHandler::close() {
  if (disk_buffer_pool_ != nullptr) {
    disk_buffer_pool_ = nullptr;
  }
}

RC RecordFileHandler::insert_record(const char *data, int record_size, RID *rid) {
  RC ret = RC::SUCCESS;
  // 找到没有填满的页面 
  int page_count = 0;
  if ((ret = disk_buffer_pool_->get_page_count(file_id_, &page_count)) != RC::SUCCESS) {
    LOG_ERROR("Failed to get page count while inserting record");
    return ret;
  }

  PageNum current_page_num = record_page_handler_.get_page_num();
  if (current_page_num < 0) {
    if (page_count >= 2) { // 当前buffer pool 有页面时才尝试加载第一页
      // 参考diskBufferPool，pageNum从1开始
      if ((ret = record_page_handler_.init(*disk_buffer_pool_, file_id_, 1)) != RC::SUCCESS) {
        LOG_ERROR("Failed to init record page handler.ret=%d", ret);
        return ret;
      }
      current_page_num = record_page_handler_.get_page_num();
    } else {
      current_page_num = 0;
    }
  }

  bool page_found = false;
  for (int i = 0; i < page_count; i++) {
    current_page_num = (current_page_num + i) % page_count; // 从当前打开的页面开始查找
    if (current_page_num == 0) {
      continue;
    }
    if (current_page_num != record_page_handler_.get_page_num()) {
      record_page_handler_.deinit();
      ret = record_page_handler_.init(*disk_buffer_pool_, file_id_, current_page_num);
      if (ret != RC::SUCCESS && ret != RC::BUFFERPOOL_INVALID_PAGE_NUM) {
        LOG_ERROR("Failed to init record page handler. page number is %d. ret=%d:%s", current_page_num, ret, strrc(ret));
        return ret;
      }
    }

    if (!record_page_handler_.is_full()) {
      page_found = true;
      break;
    }
  }

  // 找不到就分配一个新的页面
  if (!page_found) {
    BPPageHandle page_handle;
    if ((ret = disk_buffer_pool_->allocate_page(file_id_, &page_handle)) != RC::SUCCESS) {
      LOG_ERROR("Failed to allocate page while inserting record. ret=%d", ret);
      return ret;
    }

    current_page_num = page_handle.frame->page.page_num;
    record_page_handler_.deinit();
    ret = record_page_handler_.init_empty_page(*disk_buffer_pool_, file_id_, current_page_num, record_size);
    if (ret != RC::SUCCESS) {
      LOG_ERROR("Failed to init empty page. ret=%d", ret);
      disk_buffer_pool_->unpin_page(&page_handle);
      return ret;
    }
    disk_buffer_pool_->unpin_page(&page_handle);
  }

  // 找到空闲位置
  return record_page_handler_.insert_record(data, rid);
}

RC RecordFileHandler::update_record(const Record *rec) {

  RC ret = RC::SUCCESS;

  RecordPageHandler page_handler;
  if ((ret != page_handler.init(*disk_buffer_pool_, file_id_, rec->rid.page_num)) != RC::SUCCESS) {
    LOG_ERROR("Failed to init record page handler.page number=%d", rec->rid.page_num);
    return ret;
  }

  return page_handler.update_record(rec);
}

RC RecordFileHandler::delete_record(const RID *rid) {

  RC ret = RC::SUCCESS;
  RecordPageHandler page_handler;
  if ((ret != page_handler.init(*disk_buffer_pool_, file_id_, rid->page_num)) != RC::SUCCESS) {
    LOG_ERROR("Failed to init record page handler.page number=%d", rid->page_num);
    return ret;
  }
  return page_handler.delete_record(rid);
}

RC RecordFileHandler::get_record(const RID *rid, Record *rec) {
  //TODO lock?
  RC ret = RC::SUCCESS;
  if (nullptr == rid || nullptr == rec) {
    return RC::INVALID_ARGUMENT;
  }
  RecordPageHandler page_handler;
  if ((ret != page_handler.init(*disk_buffer_pool_, file_id_, rid->page_num)) != RC::SUCCESS) {
    LOG_ERROR("Failed to init record page handler.page number=%d", rid->page_num);
    return ret;
  }

  return page_handler.get_record(rid, rec);
}

////////////////////////////////////////////////////////////////////////////////
OverflowFileHandler::OverflowFileHandler() :
    disk_buffer_pool_(nullptr),
    file_id_(-1) {
}

RC OverflowFileHandler::init(DiskBufferPool &buffer_pool, int file_id) {

  RC ret = RC::SUCCESS;

  if (disk_buffer_pool_ != nullptr) {
    return RC::RECORD_OPENNED;
  }

  disk_buffer_pool_ = &buffer_pool;
  file_id_ = file_id;

  return ret;
}

void OverflowFileHandler::close() {
  if (disk_buffer_pool_ != nullptr) {
    disk_buffer_pool_ = nullptr;
  }
}

RC OverflowFileHandler::insert_record(const char *data, int data_len, OID *oid) {
  RC ret = RC::SUCCESS;
  
  int size = (data_len + OF_PAGE_DATA_SIZE - 1) / OF_PAGE_DATA_SIZE;
  PageNum *page_nums = new PageNum [size];
  OverflowPageHandler *overflow_page_handle = new OverflowPageHandler [size];

  for(int i = 0; i < size; i++) {
    BPPageHandle page_handle;
    if ((ret = disk_buffer_pool_->allocate_page(file_id_, &page_handle)) != RC::SUCCESS) {
      LOG_ERROR("Failed to allocate page while inserting record. ret=%d", ret);
      return ret;
    }
    page_nums[i] = page_handle.frame->page.page_num;
    overflow_page_handle[i].init(*disk_buffer_pool_, file_id_, page_nums[i]);
  }

  int offset = 0, cpy_len;
  for(int i = 0; i < size; i++) {
    cpy_len = (data_len - offset) >= OF_PAGE_DATA_SIZE ? OF_PAGE_DATA_SIZE : data_len - offset;
    char *page = new char [OF_PAGE_DATA_SIZE];
    memcpy(page, data+offset, cpy_len);
    ret = overflow_page_handle[i].insert_record(page, cpy_len, i == size-1 ? 0 : page_nums[i+1]);
    offset += cpy_len;
  }
  assert(offset == data_len);

  if(oid) {
    oid->first_page_num = page_nums[0];
    oid->len = data_len;
  }

  // 释放空间
  delete[] page_nums;
  delete[] overflow_page_handle;

  return ret;
}

RC OverflowFileHandler::update_record(const char *data, int data_len, const OID *old_oid, OID *new_oid) {

  RC ret = RC::SUCCESS;

  if((ret = insert_record(data, data_len, new_oid)) != RC::SUCCESS) {
    LOG_ERROR("Failed to insert new record while updating record. ret=%d", ret);
    return ret;
  }

  if((ret = delete_record(old_oid)) != RC::SUCCESS) {
    LOG_ERROR("Failed to delete old record while updating record. ret=%d", ret);
    return ret;
  }

  return ret;
}

RC OverflowFileHandler::delete_record(const OID *oid) {
  RC ret = RC::SUCCESS;
  
  int size = (oid->len + OF_PAGE_DATA_SIZE - 1) / OF_PAGE_DATA_SIZE;
  PageNum *page_nums = new PageNum [size];
  OverflowPageHandler *overflow_page_handle = new OverflowPageHandler [size];

  for(int i = 0; i < size; i++) {
    if(i == 0) page_nums[i] = oid->first_page_num;
    else page_nums[i] = overflow_page_handle[i-1].get_next_page_num();
    overflow_page_handle[i].init(*disk_buffer_pool_, file_id_, page_nums[i]);
  }

  for(int i = 0; i < size; i++) {
    ret = overflow_page_handle[i].delete_record();
  }

  // 释放空间
  delete[] page_nums;
  delete[] overflow_page_handle;

  return ret;
}

RC OverflowFileHandler::get_record(const OID *oid, char *data) {
  //TODO lock?
  RC ret = RC::SUCCESS;
  if (nullptr == oid || nullptr == data) {
    return RC::INVALID_ARGUMENT;
  }

  int size = (oid->len + OF_PAGE_DATA_SIZE - 1) / OF_PAGE_DATA_SIZE;
  PageNum *page_nums = new PageNum [size];
  OverflowPageHandler *overflow_page_handle = new OverflowPageHandler [size];

  for(int i = 0; i < size; i++) {
    if(i == 0) page_nums[i] = oid->first_page_num;
    else page_nums[i] = overflow_page_handle[i-1].get_next_page_num();
    overflow_page_handle[i].init(*disk_buffer_pool_, file_id_, page_nums[i]);
  }

  int offset = 0, cpy_len;
  for(int i = 0; i < size; i++) {
    cpy_len = (oid->len - offset) >= OF_PAGE_DATA_SIZE ? OF_PAGE_DATA_SIZE : oid->len - offset;
    char *page;
    overflow_page_handle[i].get_record(page);
    memcpy(data+offset, page, cpy_len);
    offset += cpy_len;
  }
  assert(offset == oid->len);

  // 释放空间
  delete[] page_nums;
  delete[] overflow_page_handle;

  return ret;
}

////////////////////////////////////////////////////////////////////////////////
RecordFileScanner::RecordFileScanner() : 
    disk_buffer_pool_(nullptr),
    file_id_(-1),
    condition_filter_(nullptr) {
}

RC RecordFileScanner::open_scan(DiskBufferPool & buffer_pool, int file_id, ConditionFilter *condition_filter)
{
  close_scan();

  disk_buffer_pool_ = &buffer_pool;
  file_id_ = file_id;

  condition_filter_ = condition_filter;
  return RC::SUCCESS;
}

RC RecordFileScanner::close_scan() {
  if (disk_buffer_pool_ != nullptr) {
    disk_buffer_pool_ = nullptr;
  }

  if (condition_filter_ != nullptr) {
    condition_filter_ = nullptr;
  }

  return RC::SUCCESS;
}

RC RecordFileScanner::get_first_record(Record *rec) {
  rec->rid.page_num = 1; // from 1 参考DiskBufferPool
  rec->rid.slot_num = -1;
  // rec->valid = false;
  return get_next_record(rec);
}

RC RecordFileScanner::get_next_record(Record *rec) {
  if (nullptr == disk_buffer_pool_) {
    return RC::RECORD_CLOSED;
  }

  RC ret = RC::SUCCESS;
  Record current_record = *rec;

  int page_count = 0;
  if ((ret = disk_buffer_pool_->get_page_count(file_id_, &page_count)) != RC::SUCCESS) {
    LOG_ERROR("Failed to get page count while getting next record. file id=%d", file_id_);
  }

  if (1 == page_count) {
    return RC::RECORD_EOF;
  }

  while (current_record.rid.page_num < page_count) {

    if (current_record.rid.page_num != record_page_handler_.get_page_num()) {
      record_page_handler_.deinit();
      ret = record_page_handler_.init(*disk_buffer_pool_, file_id_, current_record.rid.page_num);
      if (ret != RC::SUCCESS && ret != RC::BUFFERPOOL_INVALID_PAGE_NUM) {
        LOG_ERROR("Failed to init record page handler. page num=%d", current_record.rid.page_num);
        return ret;
      }

      if (RC::BUFFERPOOL_INVALID_PAGE_NUM == ret) {
        current_record.rid.page_num++;
        current_record.rid.slot_num = -1;
        continue;
      }
    }
    
    ret = record_page_handler_.get_next_record(&current_record);
    if (RC::SUCCESS == ret) {
      if (condition_filter_ == nullptr || condition_filter_->filter(current_record)) {
        break; // got one
      }
    } else if (RC::RECORD_EOF == ret) {
      current_record.rid.page_num++;
      current_record.rid.slot_num = -1;
    } else {
      break; // ERROR
    }
  }

  if (RC::SUCCESS == ret) {
    *rec = current_record;
  }
  return ret;
}
