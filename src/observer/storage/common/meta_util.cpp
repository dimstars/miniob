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
// Created by wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/meta_util.h"

std::string table_meta_file(const char *base_dir, const char *table_name) {
  return std::string(base_dir) + "/" + table_name + TABLE_META_SUFFIX;
}

std::string index_data_file(const char *base_dir, const char *table_name, const char *index_name) {
  return std::string(base_dir) + "/" + table_name + "-" + index_name + TABLE_INDEX_SUFFIX;
}

