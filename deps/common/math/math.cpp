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
// Created by Longda on 2010
//

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "common/math/math.h"
namespace common {

long int lrandom(const long int scope) {
  srandom(time(0));
  long int ret = 0;
  long int randNum = 0;
  if (scope < (RAND_MAX / 10)) {
    // if scope is bigger than RAND_MAX/10, the precision is not good
    randNum = random();
    ret = randNum % scope;
  } else {
    randNum = (long int)((((double)scope) / RAND_MAX) * random());
    // due to convert scope/RAND_MAX to double, it will lose precision,
    // randNum is possibly bigger scope
    ret = randNum % scope;
  }

  return ret;
}

void seed_random() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
}

} //namespace common