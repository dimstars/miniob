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

#ifndef __COMMON_MM_DEBUG_NEW_H__
#define __COMMON_MM_DEBUG_NEW_H__

#include <new>
#include <stdlib.h>
namespace common {

/* Prototypes */
bool check_leaks();
void *operator new(size_t size, const char *file, int line);
void *operator new[](size_t size, const char *file, int line);
#ifndef NO_PLACEMENT_DELETE
void operator delete(void *pointer, const char *file, int line);
void operator delete[](void *pointer, const char *file, int line);
#endif // NO_PLACEMENT_DELETE
void operator delete[](void *); // MSVC 6 requires this declaration

/* Macros */
#ifndef DEBUG_NEW_NO_NEW_REDEFINITION
#define new DEBUG_NEW
#define DEBUG_NEW new (__FILE__, __LINE__)
#define debug_new new
#else
#define debug_new new (__FILE__, __LINE__)
#endif // DEBUG_NEW_NO_NEW_REDEFINITION
#ifdef DEBUG_NEW_EMULATE_MALLOC

#define malloc(s) ((void *)(debug_new char[s]))
#define free(p) delete[](char *)(p)
#endif // DEBUG_NEW_EMULATE_MALLOC

/* Control flags */
extern bool new_verbose_flag;   // default to false: no verbose information
extern bool new_autocheck_flag; // default to true: call check_leaks() on exit

} //namespace common
#endif // __COMMON_MM_DEBUG_NEW_H__
