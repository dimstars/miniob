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
// Created by wangyunlai.wyl on 2021/5/19.
//

#include "storage/common/bplus_tree_index.h"
#include "common/log/log.h"
#include "common/lang/bitmap.h"

BplusTreeIndex::~BplusTreeIndex() noexcept {
  close();
}

RC BplusTreeIndex::create(const char *file_name, const IndexMeta &index_meta, const std::vector<const FieldMeta*> *field_meta, bool unique) {
  if (inited_) {
    return RC::RECORD_OPENNED;
  }

  RC rc = Index::init(index_meta, field_meta);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  rc = index_handler_.create(file_name, field_meta, unique);
  if (RC::SUCCESS == rc) {
    inited_ = true;
  }
  return rc;
}

RC BplusTreeIndex::open(const char *file_name, const IndexMeta &index_meta, const std::vector<const FieldMeta *> *field_meta, bool unique) {
  if (inited_) {
    return RC::RECORD_OPENNED;
  }
  RC rc = Index::init(index_meta, field_meta);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  rc = index_handler_.open(file_name, unique);
  if (RC::SUCCESS == rc) {
    inited_ = true;
  }
  return rc;
}

RC BplusTreeIndex::close() {
  if (inited_) {
    index_handler_.close();
    inited_ = false;
  }
  return RC::SUCCESS;
}

RC BplusTreeIndex::insert_entry(const char *record, const RID *rid) {
  std::vector<const char *> values;
  char bitmap_copy[RECORD_BITMAP];
  memcpy(bitmap_copy,record,RECORD_BITMAP);
  common::Bitmap bitmap(bitmap_copy,RECORD_BITMAP_BITS);
  for(int i = 0; i < field_meta_.size(); ++i){
    if(bitmap.get_bit(field_meta_[i].index())){
      values.push_back(nullptr);
    }else{
      values.push_back(record + field_meta_[i].offset());
    }
  }
  return index_handler_.insert_entry(&values, rid);
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid) {
  std::vector<const char *> values;
  char bitmap_copy[RECORD_BITMAP];
  memcpy(bitmap_copy,record,RECORD_BITMAP);
  common::Bitmap bitmap(bitmap_copy,RECORD_BITMAP_BITS);
  for(int i = 0; i < field_meta_.size(); ++i){
    if(bitmap.get_bit(field_meta_[i].index())){
      values.push_back(nullptr);
    }else{
      values.push_back(record + field_meta_[i].offset());
    }
  }
  return index_handler_.delete_entry(&values, rid);
}

IndexScanner *BplusTreeIndex::create_scanner(CompOp comp_op, std::vector<const char *> *value, std::vector<AttrType> *type) {
  BplusTreeScanner *bplus_tree_scanner = new BplusTreeScanner(index_handler_);
  RC rc = bplus_tree_scanner->open(comp_op, value, type);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete bplus_tree_scanner;
    return nullptr;
  }

  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(bplus_tree_scanner);
  return index_scanner;
}

RC BplusTreeIndex::sync() {
  return index_handler_.sync();
}

BplusTreeIndexScanner::BplusTreeIndexScanner(BplusTreeScanner *tree_scanner) :
    tree_scanner_(tree_scanner) {
}

BplusTreeIndexScanner::~BplusTreeIndexScanner() noexcept {
  tree_scanner_->close();
  delete tree_scanner_;
}

RC BplusTreeIndexScanner::next_entry(RID *rid) {
  return tree_scanner_->next_entry(rid);
}

RC BplusTreeIndexScanner::destroy() {
  delete this;
  return RC::SUCCESS;
}