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

#if !defined(__COMMON_CONF_INI_H__)
#define __COMMON_CONF_INI_H__

#include <stdio.h>

#include <iostream>
#include <map>
#include <set>
#include <string>

namespace common {

//********************************************************************
//#means comments
// Ini configuration format
//[section]
// VARNAME=VALUE

class Ini {
 public:
  /**
   * To simplify the logic, no lock's when loading configuration
   * So don't modify the data parallel
   */
  Ini();
  ~Ini();

  /**
   * load one ini configuration
   * it support load multiple ini configuration files
   * @return, 0 means success, others means failed
   */
  int load(const std::string &ini_file);

  /**
   * get the map of the section
   * if the section doesn't exist, return one empty section
   */
  const std::map<std::string, std::string> &
  get(const std::string &section = DEFAULT_SECTION);

  /**
   * get the value of the key in the section,
   * if the key-value doesn't exist,
   * use the input default_value
   */
  std::string get(const std::string &key, const std::string &default_value,
                  const std::string &section = DEFAULT_SECTION);

  /**
   * put the key-value pair to the section
   * if the key-value already exist, just replace it
   * if the section doesn't exist, it will create this section
   */
  int put(const std::string &key, const std::string &value,
          const std::string &section = DEFAULT_SECTION);

  /**
   * output all configuration to one string
   */
  void to_string(std::string &output_str);

  static const std::string DEFAULT_SECTION;

  // one line max length
  static const int MAX_CFG_LINE_LEN = 1024;

  // value split tag
  static const char CFG_DELIMIT_TAG = ',';

  // comments's tag
  static const char CFG_COMMENT_TAG = '#';

  // continue line tag
  static const char CFG_CONTINUE_TAG = '\\';

  // session name tag
  static const char CFG_SESSION_START_TAG = '[';
  static const char CFG_SESSION_END_TAG = ']';

 protected:
  /**
   * insert one empty session to sections_
   */
  void insert_session(const std::string &session_name);

  /**
   * switch session according to the session_name
   * if the section doesn't exist, it will create one
   */
  std::map<std::string, std::string> *
  switch_session(const std::string &session_name);

  /**
   * insert one entry to session_map
   * line's format is "key=value"
   *
   */
  int insert_entry(std::map<std::string, std::string> *session_map,
                  const std::string &line);

  typedef std::map<std::string, std::map<std::string, std::string>>
    SessionsMap;

 private:
  static const std::map<std::string, std::string> empty_map_;

  std::set<std::string> file_names_;
  SessionsMap sections_;
};

/**
 * Global configurate propertis
 */
Ini *&get_properties();
//********************************************************************

}// namespace common
#endif //__COMMON_CONF_INI_H__
