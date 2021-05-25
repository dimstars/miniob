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

#ifndef __COMMON_LANG_SERIALIZABLE_H__
#define __COMMON_LANG_SERIALIZABLE_H__

#include <string>
namespace common {

/**
 * Through this type to determine object type
 */
enum {
  MESSAGE_BASIC = 100,
  MESSAGE_BASIC_REQUEST = 1000,
  MESSAGE_BASIC_RESPONSE = -1000
};

class Deserializable {
public:
  /*
   * deserialize buffer to one object
   * @param[in]buffer,     buffer to store the object serialized bytes
   * @return *             object
   */
  virtual void *deserialize(const char *buffer, int bufLen) = 0;
};

class Serializable {
public:
  /*
   * serialize this object to bytes
   * @param[in] buffer,    buffer to store the object serialized bytes,
   *                       please make sure the buffer is enough
   * @param[in] bufferLen, buffer length
   * @return,              used buffer length -- success, -1 means failed
   */
  virtual int serialize(std::ostream &os) const = 0;

  /*
   * deserialize bytes to this object
   * @param[in] buffer      buffer to store the object serialized bytes
   * @param[in] bufferLen   buffer lenght
   * @return                used buffer length -- success , -1 --failed
   */
  virtual int deserialize(std::istream &is) = 0;

  /**
   * get serialize size
   * @return                >0 -- success, -1 --failed
   */
  virtual int get_serial_size() const = 0;

  /**
   * this function will generalize one output string
   */
  virtual void to_string(std::string &output) const = 0;
};

} //namespace common
#endif /* __COMMON_LANG_SERIALIZABLE_H__ */
