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

#ifndef __OBSERVER_STORAGE_COMMON_BPLUS_TREE_INDEX_H_
#define __OBSERVER_STORAGE_COMMON_BPLUS_TREE_INDEX_H_

#include "storage/common/index.h"
#include "storage/common/bplus_tree.h"

class BplusTreeIndex : public Index {
public:
  BplusTreeIndex() = default;
  virtual ~BplusTreeIndex() noexcept;

  RC create(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta, bool unique = false);
  RC open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta, bool unique = false);
  RC close();

  RC insert_entry(const char *record, const RID *rid) override;
  RC insert_key(const char *pkey, const RID *rid) override;
  RC delete_entry(const char *record, const RID *rid) override;

  IndexScanner *create_scanner(CompOp comp_op, const char *value) override;

  RC sync() override;

private:
  bool inited_ = false;
  BplusTreeHandler index_handler_;
};

class BplusTreeIndexScanner : public IndexScanner {
public:
  BplusTreeIndexScanner(BplusTreeScanner *tree_scanner);
  ~BplusTreeIndexScanner() noexcept override;

  RC next_entry(RID *rid) override;
  RC destroy() override;
private:
  BplusTreeScanner * tree_scanner_;
};

#endif //__OBSERVER_STORAGE_COMMON_BPLUS_TREE_INDEX_H_
