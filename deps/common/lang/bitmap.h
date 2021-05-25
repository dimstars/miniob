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
// Created by wangyunlai on 2021/5/7.
//

#ifndef __COMMON_LANG_BITMAP_H__
#define __COMMON_LANG_BITMAP_H__

namespace common
{

class Bitmap {
public:
  Bitmap(char *bitmap, int size);

  bool get_bit(int index);
  void set_bit(int index);
  void clear_bit(int index);

  int  next_unsetted_bit(int start);
  int  next_setted_bit(int start);

private:
  char * bitmap_;
  int    size_;
};

} // namespace common

#endif // __COMMON_LANG_BITMAP_H__