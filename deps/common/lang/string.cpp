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

#include <algorithm>
#include <ctype.h>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string.h>
#include <string>

#include "common/lang/string.h"
namespace common {

char *strip(char *str_) {
  if (str_ == NULL || *str_ == 0)
    return str_;

  char *head = str_;
  while (isspace(*head))
    ++head;

  char *last = str_ + strlen(str_) - 1;
  while (isspace(*last) && last != str_)
    --last;
  *(last + 1) = 0;
  return head;
}

void strip(std::string &str) {
  size_t head = 0;

  while (isspace(str[head])) {
    ++head;
  }

  size_t tail = str.size() - 1;
  while (isspace(str[tail]) && tail != head) {
    --tail;
  }

  str = str.substr(head, (tail - head) + 1);
}

// Translation functions with templates are defined in the header file
std::string sizeToPadStr(int size, int pad) {
  std::ostringstream ss;
  ss << std::setw(pad) << std::setfill('0') << size;
  return ss.str();
}

std::string &strToUpper(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) & std::toupper);
  return s;
}

std::string &strToLower(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) & std::tolower);
  return s;
}

void splitString(const std::string &str, std::string delim,
                 std::set<std::string> &results) {
  int cutAt;
  std::string tmpStr(str);
  while ((cutAt = tmpStr.find_first_of(delim)) != (signed)tmpStr.npos) {
    if (cutAt > 0) {
      results.insert(tmpStr.substr(0, cutAt));
    }
    tmpStr = tmpStr.substr(cutAt + 1);
  }

  if (tmpStr.length() > 0) {
    results.insert(tmpStr);
  }
}

void splitString(const std::string &str, std::string delim,
                 std::vector<std::string> &results) {
  int cutAt;
  std::string tmpStr(str);
  while ((cutAt = tmpStr.find_first_of(delim)) != (signed)tmpStr.npos) {
    if (cutAt > 0) {
      results.push_back(tmpStr.substr(0, cutAt));
    }
    tmpStr = tmpStr.substr(cutAt + 1);
  }

  if (tmpStr.length() > 0) {
    results.push_back(tmpStr);
  }
}

void splitString(char *str, char dim, std::vector<char *> &results,
                 bool keep_null) {
  char *p = str;
  char *l = p;
  while (*p) {
    if (*p == dim) {
      *p++ = 0;
      if (p - l > 1 || keep_null)
        results.push_back(l);
      l = p;
    } else
      ++p;
  }
  if (p - l > 0 || keep_null)
    results.push_back(l);
  return;
}

void mergeString(std::string &str, std::string delim,
                 std::vector<std::string> &source, size_t resultLen ){

  std::ostringstream ss;
  if (source.empty() ) {
    str = ss.str();
    return ;
  }

  if (resultLen == 0 || resultLen > source.size()) {
    resultLen = source.size();
  }

  for (int i = 0; i < resultLen; i++) {
    if (i == 0) {
      ss << source[i];
    }else {
      ss << delim << source[i];
    }

  }

  str = ss.str();
  return ;
}

void replace(std::string &str, const std::string &old,
             const std::string &newStr) {
  if (old.compare(newStr) == 0) {
    return;
  }

  if (old == "") {
    return;
  }

  if (old.length() > str.length()) {
    return;
  }

  std::string result;

  size_t index;
  size_t lastIndex = 0;

  while ((index = str.find(old, lastIndex)) != std::string::npos) {
    result += str.substr(lastIndex, index - lastIndex);
    result += newStr;
    lastIndex = index + old.length();
  }

  result += str.substr(lastIndex, str.length() - lastIndex + 1);

  str = result;

  return;
}

char *bin2hex(const char *s, const int len, char *szHexBuff) {
  int nLen = 0;
  unsigned char *pEnd = (unsigned char *)s + len;
  for (unsigned char *p = (unsigned char *)s; p < pEnd; p++) {
    nLen += sprintf(szHexBuff + nLen, "%02x", *p);
  }

  szHexBuff[nLen] = '\0';
  return szHexBuff;
}

char *hex2bin(const char *s, char *szBinBuff, int *nDestLen) {
  char buff[3];
  char *pSrc;
  int nSrcLen;
  char *pDest;
  char *pDestEnd;

  nSrcLen = strlen(s);
  if (nSrcLen == 0) {
    *nDestLen = 0;
    szBinBuff[0] = '\0';
    return szBinBuff;
  }

  *nDestLen = nSrcLen / 2;
  pSrc = (char *)s;
  buff[2] = '\0';

  pDestEnd = szBinBuff + (*nDestLen);
  for (pDest = szBinBuff; pDest < pDestEnd; pDest++) {
    buff[0] = *pSrc++;
    buff[1] = *pSrc++;
    *pDest = (char)strtol(buff, NULL, 16);
  }

  *pDest = '\0';
  return szBinBuff;
}

bool is_blank(const char *s) {
  while (*s != '\0') {
    if (!isspace(*s)) {
      return false;
    }
    s++;
  }
  return true;
}

} //namespace common
