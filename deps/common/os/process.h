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

#ifndef __COMMON_OS_PROCESS_H__
#define __COMMON_OS_PROCESS_H__
namespace common {


//! Get process Name
/**
 * @param[in]   progFullName  process full name with full path
 * @return      process_name_   process name without directory path
 */
std::string getProcessName(const char *progFullName);
//! Runs the service as a daemon
/**
 * Backgrounds the calling service as a system daemon by detaching it from
 * the controlling terminal, closes stdin, and reopens stdout and stderr
 * to the files specified in the input parmaters. "/dev/null" is accepted as
 * a valid input, which will be equivalent to closing the respective stream.
 * Keeping the streams open but reopening them allows the streams of the
 * controling terminal to be closed, thus making it possible for the terminal
 * to exit normally while the service is backgrounded. The same file
 * could be used for reopening both stderr and stdout streams.
 * Creates a new session and sets the service process as the group parent.
 *
 * @param[in]   stdOutFile  file to redirect stdout to (could be /dev/null)
 * @param[in]   stdErrFile  file to redirect stderr to (could be /dev/null)
 * @return  0 if success, error code otherwise
 */
int daemonizeService(const char *stdOutFile, const char *stdErrFile);

void sysLogRedirect(const char *stdOutFile, const char *stdErrFile);

} //namespace common
#endif //__COMMON_OS_PROCESS_H__
