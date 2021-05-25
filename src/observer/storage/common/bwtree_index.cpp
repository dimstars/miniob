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

#include "storage/common/bwtree_index.h"

#include <string.h>

#include "common/log/log.h"

/*
class GenericDataTypeComparator : public std::less<GenericDataType> {
public:
  GenericDataTypeComparator(IndexDataOperator *index_operator) : index_operator_(index_operator) {

  }
  bool operator() (const GenericDataType &p1, const GenericDataType &p2) const {
    return index_operator_->compare(p1.get_data(), p2.get_data());
  }

private:
  IndexDataOperator * index_operator_;
};

class GenericDataTypeEqualityChecker : public std::equal_to<GenericDataType> {
public:
  GenericDataTypeEqualityChecker(IndexDataOperator *index_operator) : index_operator_(index_operator) {
  }

  bool operator() (const GenericDataType &p1, const GenericDataType &p2) const {
    return 0 == index_operator_->compare(p1.get_data(), p2.get_data());
  }
private:
  IndexDataOperator *index_operator_;
};

namespace std {

template<>
class hash<GenericDataType> {
public:
  hash(IndexDataOperator *index_operator) : index_operator_(index_operator) {
  }

  size_t operator() (const GenericDataType &p) const {
    return index_operator_->hash(p.get_data());
  }

private:
  IndexDataOperator * index_operator_;
};
} // namesapce std

BwTreeIndex::BwTreeIndex() : key_size_(0), data_size_(0), key_operator_(nullptr), data_operator_(nullptr) {
}

BwTreeIndex::~BwTreeIndex() {
}

RC BwTreeIndex::init(int key_size, int data_size, IndexDataOperator *key_operator, IndexDataOperator *data_operator) {
  key_size_ = key_size;
  data_size_ = data_size;
  key_operator_ = key_operator;
  data_operator_ = data_operator;

  GenericDataTypeComparator key_comparator(key_operator);
  GenericDataTypeEqualityChecker key_equality_checker(key_operator);
  std::hash<GenericDataType> key_hasher(key_operator);
  GenericDataTypeEqualityChecker data_equality_checker(data_operator);
  std::hash<GenericDataType> data_hasher(data_operator);

  bwtree_ = new common::bwtree::BwTree<GenericDataType, GenericDataType>(
                      true, 
                      key_comparator,
                      key_equality_checker,
                      key_hasher,
                      data_equality_checker,
                      data_hasher
                    );
  LOG_INFO("Create a bwtree index");
  return RC::SUCCESS;
}

RC BwTreeIndex::insert_record(const void *key, const void *data) {
  GenericDataType key_index(key, key_size_);
  GenericDataType data_index(data, data_size_);
  bool result = bwtree_->Insert(key_index, data_index);
  if (!result) {
    LOG_ERROR("Failed to insert record "); // TODO more log
    return RC::GENERIC_ERROR;
  }
  return RC::SUCCESS;
}

RC BwTreeIndex::delete_record(const void *key, const void *data) {
  GenericDataType key_index(key, key_size_);
  GenericDataType data_index(data, data_size_);
  bool result = bwtree_->Delete(key_index, data_index);
  if (!result) {
    LOG_WARN("Failed to delete record"); // TODO more log
    return RC::GENERIC_ERROR;
  }
  return RC::SUCCESS;
}

RC BwTreeIndex::scan_key(const void *key, std::vector<const void *> &datas) {
  GenericDataType key_index(key, key_size_);
  std::vector<GenericDataType> results;
  bwtree_->GetValue(key_index, results);

  datas.reserve(results.size());

  for (const GenericDataType & result : results) {
    void *data = malloc(result.get_size()); // TODO
    memcpy(data, result.get_data(), result.get_size());
    datas.push_back((const void *)data);
  }
  return RC::SUCCESS;
}
 */