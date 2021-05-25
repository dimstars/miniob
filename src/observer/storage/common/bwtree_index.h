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
// Created by Wangyunlai on 2021/5/11.
//

#ifndef __OBSERVER_STORAGE_COMMON_BWTREE_INDEX_H_
#define __OBSERVER_STORAGE_COMMON_BWTREE_INDEX_H_

/*
#include "storage/common/index.h"

#include <vector>
#include "common/bwtree/bwtree.h"

// TODO 修改成更直观的类型
class GenericDataType {
public:
  GenericDataType() = default;
  GenericDataType(const void *data, int size) : 
      data_((const char*)data, (const char *)data + size) {
  }

  const void *get_data() const {
    return (const void *)data_.data();
  }

  int get_size() const {
    return data_.size();
  }

  GenericDataType &operator =(const GenericDataType &other)  {
    if (this != &other) {
      data_ = other.data_;
    }
    return *this;
  }
  GenericDataType &operator =(GenericDataType &&other)  {
    if (this != &other) {
      data_.swap(other.data_);
      other.data_.clear();
    }
    return *this;
  }

  GenericDataType(const GenericDataType &other) : data_(other.data_) {
  }

  GenericDataType(GenericDataType &&other) : data_(std::move(other.data_)) {
  }

  bool operator == (const GenericDataType & other) const {
    return false; // for compile
  }

  bool operator < (const GenericDataType & other) const {
    return false; // for compile
  }
private:
  std::vector<char>  data_;
};

class BwTreeIndex : public Index {
public:
  BwTreeIndex();
  virtual ~BwTreeIndex();

  RC init(int key_size, int data_size, IndexDataOperator *key_operator, IndexDataOperator *data_operator);
  
  RC insert_record(const void *key, const void *data);
  RC delete_record(const void *key, const void *data);
  RC scan_key(const void *key, std::vector<const void *> &datas);

private:
  int key_size_;
  int data_size_;
  IndexDataOperator * key_operator_;
  IndexDataOperator * data_operator_;

  common::bwtree::BwTree<GenericDataType, GenericDataType> * bwtree_;
};
 */
#endif // __OBSERVER_STORAGE_COMMON_BWTREE_INDEX_H_