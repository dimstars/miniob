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

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_H_
#define __OBSERVER_STORAGE_COMMON_INDEX_H_

#include <stddef.h>
#include <vector>

#include "rc.h"
#include "storage/common/index_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/record_manager.h"

class IndexDataOperator {
public:
  virtual ~IndexDataOperator() = default;
  virtual int compare(const void *data1, const void *data2) const = 0;
  virtual size_t hash(const void *data) const = 0;
};

class IndexScanner;

class Index {

public:
  Index() = default;
  virtual ~Index() = default;

  const IndexMeta &index_meta() const {
    return index_meta_;
  }

  virtual RC insert_entry(const char *record, const RID *rid) = 0;
  virtual RC delete_entry(const char *record, const RID *rid) = 0;

  virtual IndexScanner *create_scanner(CompOp comp_op, const char *value) = 0;

  virtual RC sync() = 0;

protected:
  RC init(const IndexMeta &index_meta, const FieldMeta &field_meta);

protected:
  IndexMeta   index_meta_;
  FieldMeta   field_meta_;    /// 当前实现仅考虑一个字段的索引
};

class IndexScanner {
public:
  IndexScanner() = default;
  virtual ~IndexScanner() = default;

  virtual RC next_entry(RID *rid) = 0;
  virtual RC destroy() = 0;
};

#endif  // __OBSERVER_STORAGE_COMMON_INDEX_H_