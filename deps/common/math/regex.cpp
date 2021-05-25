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
// Created by Longda on 2010
//

#include <regex.h>
#include <stdlib.h>
#include <sys/types.h>

#include "common/math/regex.h"
namespace common {

int regex_match(const char *str_, const char *pat_) {
  regex_t reg;
  if (regcomp(&reg, pat_, REG_EXTENDED | REG_NOSUB))
    return -1;

  int ret = regexec(&reg, str_, 0, NULL, 0);
  regfree(&reg);
  return ret;
}

} //namespace common