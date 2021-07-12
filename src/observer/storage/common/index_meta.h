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

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_META_H__
#define __OBSERVER_STORAGE_COMMON_INDEX_META_H__

#include <string>
#include <vector>
#include "rc.h"
#include "sql/parser/parse_defs.h"

class TableMeta;
class FieldMeta;

namespace Json {
class Value;
} // namespace Json

class IndexMeta {
public:
  IndexMeta() = default;

  RC init(const char *name, std::vector<const FieldMeta*> &fields, bool unique, int field_num);

public:
  const char *name() const;
  const char *field(int pos) const;
  const char *extended_attr() const;
  int field_num() const;

  void desc(std::ostream &os) const;
public:
  void to_json(Json::Value &json_value) const;
  static RC from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index);

private:
  std::string               name_;
  std::string               fields_[MAX_NUM];
  std::string               extended_attr_; ///包含"unique"时表示唯一索引
  int                       field_num_;
};
#endif // __OBSERVER_STORAGE_COMMON_INDEX_META_H__