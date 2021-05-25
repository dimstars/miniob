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

#ifndef __COMMON_MM_MEM_H__
#define __COMMON_MM_MEM_H__

#include <stdlib.h>
#include <string.h>

#include <new>

#include "common/defs.h"
namespace common {

#ifndef MEM_DEBUG
#define lcalloc calloc
#define lmalloc malloc
#define lfree free
#define lrealloc realloc

#else



typedef struct MemID_t {
public:
  const static int MEM_FILENAME_LEN = 32;
  struct MemID_t *mNext;
  char mFile[MEM_FILENAME_LEN];
  u64_t mSize;
  u32_t mLine;
} MemID;

class CLMemTrace {
public:
  static void *malloc(size_t size, const char *file, const int line,
                      bool retry = false) throw(std::bad_alloc);

  // just use for realloc, same functionality as realloc
  static void *realloc(void *ptr, size_t size, const char *file,
                       const int line);

  static void free(void *ptr);

  static void output();

  /**
   * set whether show every details
   */
  static void setVerbose(bool verbose) { mVerbose = verbose; }

protected:
  static std::new_handler getNewHandler();

  /**
   * Don't use stl due to map or string will use new,
   * so it will leading to dead loop
   */
  // static std::map<void *, CMemID> mMemIds;
  const static int MEM_HASHTABLE_SIZE = 16384;

  static MemID *mMemIDs[MEM_HASHTABLE_SIZE];
  static u64_t mUsedSize;
  static pthread_mutex_t mMutex;
  static bool mVerbose;
};

#define lcalloc(nmemb, size) Lcalloc(nmemb, size, __FILE__, __LINE__)

#define lmalloc(size) Lmalloc(size, __FILE__, __LINE__)

#define lrealloc(ptr, size) Lrealloc(ptr, size, __FILE__, __LINE__)

#define lfree(ptr) Lfree(ptr)

void *Lcalloc(size_t nmemb, size_t size, const char *file, const int line);
void *Lmalloc(size_t size, const char *file, const int line);
void Lfree(void *ptr);
void *Lrealloc(void *ptr, size_t size, const char *file, const int line);

/**
 * My own operator new function, it will record alloc information
 * default alloc memory or DEBUG_NEW will go to this function
 */
static void *operator new(std::size_t size, const char *file, int line);
static void *operator new[](std::size_t size, const char *file, int line);

/**
 * when user use debug_new, then it will go to this function
 */
static void *operator new(std::size_t size) throw(std::bad_alloc);
static void *operator new[](std::size_t size) throw(std::bad_alloc);

/**
 * when user use nothrow 's new function, it will go this function
 */
static void *operator new(std::size_t size, const std::nothrow_t &) throw();
static void *operator new[](std::size_t size, const std::nothrow_t &) throw();

/**
 * my own operator function, low level function
 */
static void operator delete(void *pointer, const char *file, int line);
static void operator delete[](void *pointer, const char *file, int line);

/**
 * the default delete function
 */
static void operator delete(void *pointer);
static void operator delete[](void *pointer);

///**
// * nothrow 's delete function, it will be the pair of nothrow's new function
// */
// void operator delete(void* pointer, const std::nothrow_t&);
// void operator delete[](void* pointer, const std::nothrow_t&);

#define new DEBUG_NEW
#define DEBUG_NEW new (__FILE__, __LINE__)
#define debug_new new

#endif /* MEM_DEBUG */

} //namespace common
#endif /* __COMMON_MM_MEM_H__ */
