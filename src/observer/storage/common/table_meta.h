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

#ifndef __OBSERVER_STORAGE_COMMON_TABLE_META_H__
#define __OBSERVER_STORAGE_COMMON_TABLE_META_H__

#include <string>
#include <vector>

#include "rc.h"
#include "storage/common/field_meta.h"
#include "storage/common/index_meta.h"
#include "common/lang/serializable.h"

class TableMeta : public common::Serializable {
public:
  TableMeta() = default;
  ~TableMeta() = default;

  TableMeta(const TableMeta &other);

  void swap(TableMeta &other) noexcept;

  RC init(const char *name, int field_num, const AttrInfo attributes[]);

  RC add_index(const IndexMeta &index);

public:
  const char * name() const;
  const FieldMeta * trx_field() const;
  const FieldMeta * field(int index) const;
  const FieldMeta * field(const char *name) const;
  const FieldMeta * field(const char *name, int len) const;
  const FieldMeta * find_field_by_offset(int offset) const;
  int field_num() const;
  int sys_field_num() const;

  const IndexMeta * index(const char *name) const;
  /*
  * 查找第一级key是field,且key级数最少的index。
  */
  const IndexMeta * find_index_by_field(const char *field) const;
  /*
  * 检查key顺序为field_names的index是否存在。
  */
  bool check_index_existance(const char * const *field_names, int field_num);
  /*
  * 查找前n级key在prefix_field_names中,最后一级key在prefix_field_names中或last_field_names中，匹配长度最长的index。多个index匹配长度相同时返回key级数最少的。
  */
  const IndexMeta * find_index_by_field(std::vector<const char *> *prefix_field_names,std::vector<const char *> *last_field_names) const;
  const IndexMeta * index(int i) const;
  int index_num() const;

  int record_size() const;
  bool overflow_exist() const;

public:
  int  serialize(std::ostream &os) const override;
  int  deserialize(std::istream &is) override;
  int  get_serial_size() const override;
  void to_string(std::string &output) const override;
  void desc(std::ostream &os) const;

private:
  static RC init_sys_fields();
private:
  std::string   name_;
  std::vector<FieldMeta>  fields_; // 包含sys_fields
  std::vector<IndexMeta>  indexes_;

  int  record_size_ = 0;
  bool  overflow_exist_ = 0;

  static std::vector<FieldMeta> sys_fields_;
};

#endif // __OBSERVER_STORAGE_COMMON_TABLE_META_H__