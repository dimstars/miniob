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

#ifndef __COMMON_SEDA_CLASS_FACTORY_H__
#define __COMMON_SEDA_CLASS_FACTORY_H__

#include <list>

#include "common/defs.h"
#include "common/log/log.h"
namespace common {


//! A class to construct arbitrary subclass instances
/**
 *
 *  This class provides a general solution to constructing instances
 *  of subclasses from a common generic base.  The template is
 *  instantiated for each generic base class.  Then each new subclass
 *  class creates a corresponding ClassFactory instance which includes
 *  a tag which identifies the class and a factory function which
 *  constructs an instance of the class when invoked.  The factory
 *  function takes the tag and a properties object as parameters.
 *
 *  Each class factory instance MUST be constructed before the first
 *  call to makeInstance().  Otherwise, the entry will not be found
 *  on the list.  To ensure this, if the class factory instance is
 *  provided by a library, it should be declared with static linkage
 *  within the library initialization routine.  If the class factory
 *  instance is provided by the main application, it should be declared
 *  with static linkage in a global initialization routine.
 */

template<class T>
class ClassFactory {

 public:
  typedef T *(*FactoryFunc)(const std::string &);

  //! Constructor
  /**
   * @param[in] tag     Tag identifies a particular sub-class
   * @param[in] func    Factory function to create sub-class instance
   *
   * @post (tag,func) pair is entered into global factory list
   */
  ClassFactory(const std::string &tag, FactoryFunc func);

  //! Destructor
  ~ClassFactory();

  //! Construct an instance of a specified sub-class
  /**
   * @param[in] tag     Identifies sub-class to instantiate
   * @param[in] prop    Properties desired for the instance
   *
   * @return a reference to the desired instance
   */
  static T *makeInstance(const std::string &tag);

 private:
  //! Accessor function that gets the head of the factory list
  static ClassFactory<T> *&factListHead();

  std::string identifier; //!< identifier for this factory
  FactoryFunc factFunc;   //!< factory function for this class
  ClassFactory<T> *next;  //!< next factory in global list
};

//! Accessor function that gets the head of the factory list
/**
 * Implementation notes:
 * The head pointer in the list must be initialized to NULL before
 * it is accessed from any ClassFactory<T> constructor.  We cannot
 * rely on C++ constructor order to achieve this.  Instead, we wrap
 * the list head in an accessor function, and declare its linkage
 * as static.  C++ guarantees that the first time the function is
 * invoked (from anywhere) the static local will be initialized.
 */
template<class T>
ClassFactory<T> *&ClassFactory<T>::factListHead() {
  static ClassFactory<T> *factList = NULL;
  return factList;
}

//! Constructor
/**
 * Implementation notes:
 * constructor places current instance on the global factory list.
 */
template<class T>
ClassFactory<T>::ClassFactory(const std::string &tag, FactoryFunc func)
  : identifier(tag), factFunc(func) {
  next = factListHead();
  factListHead() = this;
}

//! Destructor
template<class T>
ClassFactory<T>::~ClassFactory() {}

//! Construct an instance of a specified sub-class
/**
 * Implementation notes:
 * scan global list to find matching tag and use the factory func to
 * create an instance.
 */
template<class T>
T *ClassFactory<T>::makeInstance(const std::string &tag) {
  T *instance = NULL;
  ClassFactory<T> *current = factListHead();

  // search the global factory list for a match
  while ((current != NULL) && (tag != current->identifier)) {
    current = current->next;
  }

  // if we have a match, create and return an instance
  if (current != NULL) {
    FactoryFunc fptr = current->factFunc;
    instance = (*fptr)(tag);
  }

  ASSERT((instance != NULL), "%s%s", tag.c_str(), "instance not created");
  return instance;
}

} //namespace common
#endif // __COMMON_SEDA_CLASS_FACTORY_H__
