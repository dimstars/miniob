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

#ifndef __OBSERVER_STORAGE_COMMON_DB_H__
#define __OBSERVER_STORAGE_COMMON_DB_H__

#include <vector>
#include <string>
#include <unordered_map>

#include "rc.h"
#include "sql/parser/parse_defs.h"

class Table;

class Db {
public:
  Db() = default;
  ~Db();

  RC init(const char *name, const char *dbpath);

  RC create_table(const char *table_name, int attribute_count, const AttrInfo *attributes);

  Table *find_table(const char *table_name) const;

  const char *name() const;

  void all_tables(std::vector<std::string> &table_names) const;

  RC sync();
private:
  RC open_all_tables();

private:
  std::string   name_;
  std::string   path_;
  std::unordered_map<std::string, Table *>  opened_tables_;
};

#endif // __OBSERVER_STORAGE_COMMON_DB_H__