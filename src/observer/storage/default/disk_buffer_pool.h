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
// Created by Longda on 2021/4/13.
//
#ifndef __OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_
#define __OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#include <string>
#include <sys/stat.h>
#include <time.h>

#include <vector>
#include <unordered_map>
#include <map>
#include "rc.h"

typedef int PageNum;

#define BP_INVALID_PAGE_NUM (-1)
#define BP_PAGE_SIZE (1 << 12)
#define BP_PAGE_DATA_SIZE (BP_PAGE_SIZE - sizeof(PageNum))
#define OF_PAGE_DATA_SIZE (BP_PAGE_DATA_SIZE - sizeof(PageNum)) // page可写数据部分需要减去OFPageHeader(一个page num大小)
#define BP_FILE_SUB_HDR_SIZE (sizeof(BPFileSubHeader))
#define BP_BUFFER_SIZE 50
#define MAX_OPEN_FILE 50
class BPManager;
class DiskBufferPool;
typedef struct {
  PageNum page_num;
  char data[BP_PAGE_DATA_SIZE];
} Page;
// sizeof(Page) should be equal to BP_PAGE_SIZE

// 第一页是特殊页
// 包括BPFileSubHeader+Page，Page包括PageNum+一个bitmap
typedef struct {
  PageNum page_count;
  int allocated_pages;
} BPFileSubHeader;

typedef struct {
  bool dirty;
  unsigned int pin_count;
  unsigned long acc_time;
  int file_desc;
  Page page;
  std::string fname;
} Frame;

typedef struct {
  bool open;
  Frame *frame;
} BPPageHandle;

class BPFileHandle{
public:
  BPFileHandle() {
    memset(this, 0, sizeof(*this));
  }

public:
  bool bopen;
  const char *file_name;
  int file_desc;
  Frame *hdr_frame;
  Page *hdr_page;
  char *bitmap;   // 第一个page的bitmap作为所有page的bitmap // 每个page的bitmap作为record的bitmap
  BPFileSubHeader *file_sub_header;
} ;

typedef struct _LruItem{
  _LruItem *pre;
  _LruItem *next;
  int val;  //bpmanager中frame的位序
}LruItem;

class BPManager {
public:
  BPManager(int size = BP_BUFFER_SIZE);

  ~BPManager();

  RC alloc(Frame **buf);

  Frame* alloc();

  RC open_file(const char *fname, BPFileHandle **fhandle);

  Frame *get(const char *fname, PageNum page_num);

  void dispose_block(Frame *f);

  void insert_page(Frame *f);

  RC dispose_page(const char *fname, PageNum page_num);

  RC dispose_page(int desc, PageNum page_num);

  RC dispose_all_pages(const char *fname);

  RC force_page(const char *fname, PageNum page_num);

  RC force_all_pages(const char *fname);

  Frame *getFrame() { return frame_; }

  RC close_file(const char *fname);

  RC flush_block(Frame *frame);

  RC load_page(PageNum page_num, BPFileHandle *file_handle, Frame *frame);

  void print();

  void unpin_page(const char *file_name, int page_num);
private:
  int size;
  Frame *frame_ = nullptr;
  BPFileHandle *open_list_[MAX_OPEN_FILE];
  LruItem *items_;
  LruItem *head_;
  LruItem *tail_;
  LruItem *free_list_;
  std::map<std::string, int> file_map_;
  std::unordered_map<int, std::unordered_map<int, LruItem*> > hash_map_;
};

// TODO refactor
class DiskBufferPool {
public:
  /**
  * 创建一个名称为指定文件名的分页文件
  */
  RC create_file(const char *file_name);

  /**
   * 根据文件名打开一个分页文件，返回文件ID
   * @return
   */

  /**
   * 关闭fileID对应的分页文件
   */
  RC close_file(const char *file_name);

  /**
   * 根据文件ID和页号获取指定页面到缓冲区，返回页面句柄指针。
   * @return
   */
  RC get_this_page(const char *file_name, PageNum page_num, BPPageHandle *page_handle);

  /**
   * 在指定文件中分配一个新的页面，并将其放入缓冲区，返回页面句柄指针。
   * 分配页面时，如果文件中有空闲页，就直接分配一个空闲页；
   * 如果文件中没有空闲页，则扩展文件规模来增加新的空闲页。
   */
  RC allocate_page(const char *file_name, BPPageHandle *page_handle);

  /**
   * 根据页面句柄指针返回对应的页面号
   */
  RC get_page_num(BPPageHandle *page_handle, PageNum *page_num);

  /**
   * 根据页面句柄指针返回对应的数据区指针
   */
  RC get_data(BPPageHandle *page_handle, char **data);

  /**
   * 丢弃文件中编号为pageNum的页面，将其变为空闲页
   */
  RC dispose_page(const char *file_name, PageNum page_num);

  /**
   * 标记指定页面为“脏”页。如果修改了页面的内容，则应调用此函数，
   * 以便该页面被淘汰出缓冲区时系统将新的页面数据写入磁盘文件
   */
  RC mark_dirty(BPPageHandle *page_handle);

  /**
   * 此函数用于解除pageHandle对应页面的驻留缓冲区限制。
   * 在调用GetThisPage或AllocatePage函数将一个页面读入缓冲区后，
   * 该页面被设置为驻留缓冲区状态，以防止其在处理过程中被置换出去，
   * 因此在该页面使用完之后应调用此函数解除该限制，使得该页面此后可以正常地被淘汰出缓冲区
   */
  RC unpin_page(BPPageHandle *page_handle);

  RC unpin_page(const char *file_name, int page_num);

  /**
   * 获取文件的总页数
   */
  RC get_page_count(const char *file_name, int *page_count);

  RC flush_all_pages(const char *file_name);

  RC dispose_all_pages(const char *file_name);

protected:
  RC allocate_block(Frame **buf);
  RC dispose_block(Frame *buf);
  RC open_file(const char *file_name);
  /**
   * 刷新指定文件关联的所有脏页到磁盘，除了pinned page
   * @param file_handle
   * @param page_num 如果不指定page_num 将刷新所有页
   */
  RC force_page(const char *file_name, PageNum page_num);
  RC force_all_pages(const char *file_name);
  RC check_page_num(PageNum page_num, BPFileHandle *file_handle);
private:
  BPManager bp_manager_;
};

DiskBufferPool *theGlobalDiskBufferPool();

#endif //__OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_
