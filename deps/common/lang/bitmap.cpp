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

#include "common/lang/bitmap.h"

namespace common
{

int find_first_zero(char byte, int start) {
  for (int i = start; i < 8; i++) {
    if ((byte & (1 << i)) == 0) {
      return i;
    }
  }
  return -1;
}

int find_first_setted(char byte, int start) {
  for (int i = start; i < 8; i++) {
    if ((byte & (1 << i)) != 0) {
      return i;
    }
  }
  return -1;
}

Bitmap::Bitmap(char *bitmap, int size) : bitmap_(bitmap), size_(size) {
}

bool Bitmap::get_bit(int index) {
  char bits = bitmap_[index / 8];
  return (bits & (1 << (index % 8))) != 0;
}

void Bitmap::set_bit(int index) {
  char &bits = bitmap_[index / 8];
  bits |= (1 << (index % 8));
}

void Bitmap::clear_bit(int index) {
  char &bits = bitmap_[index / 8];
  bits &= ~(1 << (index % 8));
}

int Bitmap::next_unsetted_bit(int start) {
  int ret = -1;
  int start_in_byte = start % 8;
  for (int iter = start / 8, end = size_ / 8; iter < end; iter++) {
    char byte = bitmap_[iter];
    if (byte != -1) {
      int index_in_byte = find_first_zero(byte, start_in_byte);
      if (index_in_byte >= 0) {
        ret = iter * 8 + index_in_byte;
        break;
      }

      start_in_byte = 0;
    }
  }

  if (ret >= size_) {
    ret = -1;
  }
  return ret;
}

int Bitmap::next_setted_bit(int start) { // TODO
  int ret = -1;
  int start_in_byte = start % 8;
  for (int iter = start / 8, end = size_ / 8; iter < end; iter++) {
    char byte = bitmap_[iter];
    if (byte != 0x00) {
      int index_in_byte = find_first_setted(byte, start_in_byte);
      if (index_in_byte >= 0) {
        ret = iter * 8 + index_in_byte;
        break;
      }

      start_in_byte = 0;
    }
  }

  if (ret >= size_) {
    ret = -1;
  }
  return ret;
}

} // namespace common