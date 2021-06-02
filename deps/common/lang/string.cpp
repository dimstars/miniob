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

#include "common/lang/string.h"

#include <ctype.h>
#include <errno.h>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "common/log/log.h"
namespace common {

char *strip(char *str_) {
  if (str_ == NULL || *str_ == 0){
    LOG_ERROR("The augument is invalid!");
    return str_;
  }

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
std::string size_to_pad_str(int size, int pad) {
  std::ostringstream ss;
  ss << std::setw(pad) << std::setfill('0') << size;
  return ss.str();
}

std::string &str_to_upper(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) & std::toupper);
  return s;
}

std::string &str_to_lower(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) & std::tolower);
  return s;
}

void split_string(const std::string &str, std::string delim,
                 std::set<std::string> &results) {
  int cut_at;
  std::string tmp_str(str);
  while ((cut_at = tmp_str.find_first_of(delim)) != (signed)tmp_str.npos) {
    if (cut_at > 0) {
      results.insert(tmp_str.substr(0, cut_at));
    }
    tmp_str = tmp_str.substr(cut_at + 1);
  }

  if (tmp_str.length() > 0) {
    results.insert(tmp_str);
  }
}

void split_string(const std::string &str, std::string delim,
                 std::vector<std::string> &results) {
  int cut_at;
  std::string tmp_str(str);
  while ((cut_at = tmp_str.find_first_of(delim)) != (signed)tmp_str.npos) {
    if (cut_at > 0) {
      results.push_back(tmp_str.substr(0, cut_at));
    }
    tmp_str = tmp_str.substr(cut_at + 1);
  }

  if (tmp_str.length() > 0) {
    results.push_back(tmp_str);
  }
}

void split_string(char *str, char dim, std::vector<char *> &results,
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

void merge_string(std::string &str, std::string delim,
                 std::vector<std::string> &source, size_t result_len){

  std::ostringstream ss;
  if (source.empty() ) {
    str = ss.str();
    return;
  }

  if (result_len == 0 || result_len > source.size()) {
    result_len = source.size();
  }

  for (int i = 0; i < result_len; i++) {
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
             const std::string &new_str) {
  if (old.compare(new_str) == 0) {
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
  size_t last_index = 0;

  while ((index = str.find(old, last_index)) != std::string::npos) {
    result += str.substr(last_index, index - last_index);
    result += new_str;
    last_index = index + old.length();
  }

  result += str.substr(last_index, str.length() - last_index + 1);

  str = result;

  return;
}

char *bin2hex(const char *s, const int len, char *sz_hex_buff) {
  int n_len = 0;
  unsigned char *p_end = (unsigned char *)s + len;
  for (unsigned char *p = (unsigned char *)s; p < p_end; p++) {
    n_len += sprintf(sz_hex_buff + n_len, "%02x", *p);
  }

  sz_hex_buff[n_len] = '\0';
  return sz_hex_buff;
}

char *hex2bin(const char *s, char *sz_bin_buff, int *n_destLen) {
  char buff[3];
  char *p_src;
  int n_srcLen;
  char *p_dest;
  char *p_dest_end;

  n_srcLen = strlen(s);
  if (n_srcLen == 0) {
    *n_destLen = 0;
    sz_bin_buff[0] = '\0';
    return sz_bin_buff;
  }

  *n_destLen = n_srcLen / 2;
  p_src = (char *)s;
  buff[2] = '\0';

  p_dest_end = sz_bin_buff + (*n_destLen);
  for (p_dest = sz_bin_buff; p_dest < p_dest_end; p_dest++) {
    buff[0] = *p_src++;
    buff[1] = *p_src++;
    *p_dest = (char)strtol(buff, NULL, 16);
  }

  *p_dest = '\0';
  return sz_bin_buff;
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
