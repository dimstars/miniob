/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
OceanBase is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Longda on 2010.
//

#include <thread>

#include "common/defs.h"
#include "common/os/os.h"

namespace common {
// Don't care windows
u32_t getCpuNum() {
  return std::thread::hardware_concurrency();
}

}//namespace common