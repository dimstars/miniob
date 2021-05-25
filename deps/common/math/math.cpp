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

void seedRandom() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
}

} //namespace common