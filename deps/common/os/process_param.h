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

#ifndef __COMMON_OS_PROCESS_PARAM_H__
#define __COMMON_OS_PROCESS_PARAM_H__

#include <string>
#include <vector>
namespace common {

class ProcessParam {

 public:
  ProcessParam() {}

  virtual ~ProcessParam() {}

  void init_default(std::string &process_name);

  const std::string &get_std_out() const { return std_out_; }

  void set_std_out(const std::string &std_out) { ProcessParam::std_out_ = std_out; }

  const std::string &get_std_err() const { return std_err_; }

  void set_std_err(const std::string &std_err) { ProcessParam::std_err_ = std_err; }

  const std::string &get_conf() const { return conf; }

  void set_conf(const std::string &conf) { ProcessParam::conf = conf; }

  const std::string &get_process_name() const { return process_name_; }

  void set_process_name(const std::string &processName) {
    ProcessParam::process_name_ = processName;
  }

  bool is_demon() const { return demon; }

  void set_demon(bool demon) { ProcessParam::demon = demon; }

  const std::vector<std::string> &get_args() const { return args; }

  void set_args(const std::vector<std::string> &args) {
    ProcessParam::args = args;
  }

  void set_server_port(int port) {
    server_port_ = port;
  }

  int get_server_port() const {
    return server_port_;
  }

 private:
  std::string std_out_;            // The output file
  std::string std_err_;            // The err output file
  std::string conf;              // The configuration file
  std::string process_name_;       // The process name
  bool demon;                    // whether demon or not
  std::vector<std::string> args; // arguments
  int server_port_ = -1;         // server port(if valid, will overwrite the port in the config file)
};

ProcessParam*& the_process_param();

} //namespace common
#endif //__COMMON_OS_PROCESS_PARAM_H__
