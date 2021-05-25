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

#include "process_param.h"
#include <assert.h>
namespace common {

//! Global process config
ProcessParam*& the_process_param()
{
  static ProcessParam* process_cfg = new ProcessParam();

  return process_cfg;
}

void ProcessParam::init_default(std::string &process_name) {
  assert(process_name.empty() == false);
  this->process_name_ = process_name;
  if (std_out_.empty()) {
    std_out_ = "../log/" + process_name + ".out";
  }
  if (std_err_.empty()) {
    std_err_ = "../log/" + process_name + ".err";
  }
  if (conf.empty()) {
    conf = "../etc/" + process_name + ".ini";
  }

  demon = false;
}



} //namespace common