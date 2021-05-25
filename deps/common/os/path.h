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
// Created by Longda on 2021/3/27.
//

#ifndef __COMMON_OS_PATH_H__
#define __COMMON_OS_PATH_H__

#include <string>
namespace common {


/**
 * get file name from full path
 * example
 * input: /test/happy/  --> return : ""
 * input: /test/happy   --> return : happy
 * input: test/happy    --> return : happy
 * input: happy         --> return : happy
 * input: ""            --> return : ""
 */
std::string getFileName(const std::string &fullPath);
void getFileName(const char *path, std::string &fileName);

/**
 * get file path from full path
 * example
 * input: /test/happy   --> return : /test
 * input: test/happy    --> return : test
 * input: happy         --> return : happy
 * input: ""            --> return : ""
 */
std::string getFilePath(const std::string &fullPath);
void getDirName(const char *path, std::string &parent);

/**
 *  Get absolute path
 * input: path
 * reutrn absolutely path
 */
std::string getAboslutPath(const char *path);

/**
 * 判断给定目录是否是文件夹
 */
bool is_directory(const char *path);

/**
 * 判断指定路径是否存在并且是文件夹
 * 如果不存在将会逐级创建
 * @return 创建失败，或者不是文件夹将会返回失败
 */
bool check_directory(std::string &path);

/**
 * 列出指定文件夹下符合指定模式的所有文件
 * @param filter_pattern  示例 ^miniob.*bin$
 * @return 成功返回找到的文件个数，否则返回-1
 */
int list_file(const char *path, const char *filter_pattern, std::vector<std::string> &files); // TODO refer io/io.h::getFileList

} //namespace common
#endif //__COMMON_OS_PATH_H__
