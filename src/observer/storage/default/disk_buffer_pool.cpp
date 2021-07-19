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
#include "disk_buffer_pool.h"
#include <errno.h>
#include <string.h>

#include "common/log/log.h"

using namespace common;

unsigned long current_time() {
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC, &tp);
  return tp.tv_sec * 1000 * 1000 * 1000UL + tp.tv_nsec;
}

DiskBufferPool* theGlobalDiskBufferPool() {
  static DiskBufferPool *instance = new DiskBufferPool();

  return instance;
}

RC DiskBufferPool::create_file(const char *file_name) {
  int fd = open(file_name, O_RDWR | O_CREAT | O_EXCL ,
                 S_IREAD | S_IWRITE);
  if (fd < 0) {
    LOG_ERROR("Failed to create %s, due to %s.", file_name, strerror(errno));
    return RC::SCHEMA_DB_EXIST;
  }

  close(fd);

  /**
   * Here don't care about the failure
   */
  fd = open(file_name, O_RDWR);

  Page page;
  memset(&page, 0, sizeof(Page));

  BPFileSubHeader *fileSubHeader;
  fileSubHeader = (BPFileSubHeader *)page.data;
  fileSubHeader->allocated_pages = 1;
  fileSubHeader->page_count = 1;
  char *bitmap = page.data + (int)BP_FILE_SUB_HDR_SIZE;
  bitmap[0] |= 0x01;
  if (lseek(fd, 0, SEEK_SET) == -1) {
    LOG_ERROR("Failed to seek file %s to position 0, due to %s .", file_name,
              strerror(errno));
    return RC::IOERR_SEEK;
  }

  if (write(fd, (char *)&page, sizeof(Page)) != sizeof(Page)) {
    LOG_ERROR("Failed to write header to file %s, due to %s.", file_name,
              strerror(errno));
    close(fd);
    return RC::IOERR_WRITE;
  }

  close(fd);
  LOG_INFO("Successfully create %s.", file_name);
  return RC::SUCCESS;
}

RC DiskBufferPool::open_file(const char *file_name) {
  BPFileHandle *p;
  return bp_manager_.open_file(file_name,&p);
  // TODO,  This part isn't gentle, the better method is using LRU queue.
  
}

RC DiskBufferPool::close_file(const char *file_name) {
  RC tmp;
  BPFileHandle *file_handle = nullptr;
  tmp = bp_manager_.open_file(file_name, &file_handle);
  if(RC::SUCCESS != tmp)
    return RC::SUCCESS;
  file_handle->hdr_frame->pin_count--;
  if ((tmp = force_all_pages(file_name)) != RC::SUCCESS) {
    file_handle->hdr_frame->pin_count++;
    LOG_ERROR("Failed to closeFile %s, due to failed to force all pages.",
              file_handle->file_name);
    return tmp;
  }

  if (close(file_handle->file_desc) < 0) {
    LOG_ERROR("Failed to close file:%s, fileName:%s, error:%s", file_name,
              file_handle->file_name, strerror(errno));
    return RC::IOERR_CLOSE;
  }
  bp_manager_.close_file(file_name);
  return RC::SUCCESS;
}

RC DiskBufferPool::get_this_page(const char *file_name, PageNum page_num, BPPageHandle *page_handle) {
  RC tmp;

  BPFileHandle *file_handle = nullptr;
  RC rc = bp_manager_.open_file(file_name, &file_handle);
  if(RC::SUCCESS != rc)
    return rc;
  if ((tmp = check_page_num(page_num, file_handle)) != RC::SUCCESS) {
    LOG_ERROR("Failed to load page %s:%d, due to invalid pageNum.",
              file_handle->file_name, page_num);
    return tmp;
  }
  Frame *f = bp_manager_.get(file_name,page_num);
  if(f){
    // This page has been loaded.
    page_handle->frame = f;
    page_handle->frame->pin_count++;
    page_handle->frame->acc_time = current_time();
    page_handle->open = true;
    return RC::SUCCESS;
  }
  rc = bp_manager_.alloc(&f);
  if(rc != RC::SUCCESS){
    LOG_ERROR("Failed to load page %s:%d, due to failed to alloc page.",
              file_handle->file_name, page_num);
    return RC::NOMEM;
  }

  page_handle->frame = f;
  page_handle->frame->dirty = false;
  page_handle->frame->file_desc = file_handle->file_desc;
  page_handle->frame->pin_count = 1;
  page_handle->frame->acc_time = current_time();
  //bp_manager_.print();
 // LOG_INFO("get page :index:%d %s %d count %d",page_handle->frame - bp_manager_.getFrame(),page_handle->frame->fname.c_str(),page_handle->frame->page.page_num,page_handle->frame->pin_count);
  if ((tmp = bp_manager_.load_page(page_num, file_handle, page_handle->frame)) !=
      RC::SUCCESS) {
    LOG_ERROR("Failed to load page %s:%d", file_handle->file_name);
    page_handle->frame->pin_count = 0;
    dispose_block(page_handle->frame);
    return tmp;
  }
  bp_manager_.insert_page(page_handle->frame);
  page_handle->open = true;
  return RC::SUCCESS;
}

RC DiskBufferPool::allocate_page(const char *file_name, BPPageHandle *page_handle) {
  RC tmp;

  BPFileHandle *file_handle = nullptr;
  tmp = bp_manager_.open_file(file_name, &file_handle);
  if(tmp != RC::SUCCESS)
    return tmp;

  int byte = 0, bit = 0;
  if ((file_handle->file_sub_header->allocated_pages) <
      (file_handle->file_sub_header->page_count)) {
    // There is one free page
    for (int i = 0; i < file_handle->file_sub_header->page_count; i++) {
      byte = i / 8;
      bit = i % 8;
      if (((file_handle->bitmap[byte]) & (1 << bit)) == 0) {
        (file_handle->file_sub_header->allocated_pages)++;
        file_handle->bitmap[byte] |= (1 << bit);
        return get_this_page(file_name, i, page_handle);
      }
    }

  }

  if ((tmp = bp_manager_.alloc(&(page_handle->frame))) != RC::SUCCESS) {
    LOG_ERROR("Failed to allocate page %s, due to no free page.",
              file_handle->file_name);
    return tmp;
  }

  PageNum page_num = file_handle->file_sub_header->page_count;
  file_handle->file_sub_header->allocated_pages++;
  file_handle->file_sub_header->page_count++;

  byte = page_num / 8;
  bit = page_num % 8;
  file_handle->bitmap[byte] |= (1 << bit);
  file_handle->hdr_frame->dirty = true;

  page_handle->frame->dirty = false;
  page_handle->frame->file_desc = file_handle->file_desc;
  page_handle->frame->pin_count = 1;
  page_handle->frame->acc_time = current_time();
  memset(&(page_handle->frame->page), 0, sizeof(Page));
  page_handle->frame->fname = file_name;
  page_handle->frame->page.page_num = file_handle->file_sub_header->page_count - 1;
  
  bp_manager_.insert_page(page_handle->frame);

  // Use flush operation to extion file
  if ((tmp = bp_manager_.flush_block(page_handle->frame)) != RC::SUCCESS) {
    LOG_ERROR("Failed to alloc page %s , due to failed to extend one page.",
              file_handle->file_name);
    return tmp;
  }

  page_handle->open = true;
  return RC::SUCCESS;
}

RC DiskBufferPool::get_page_num(BPPageHandle *page_handle, PageNum *page_num) {
  if (!page_handle->open)
    return RC::BUFFERPOOL_CLOSED;
  *page_num = page_handle->frame->page.page_num;
  return RC::SUCCESS;
}

RC DiskBufferPool::get_data(BPPageHandle *page_handle, char **data) {
  if (!page_handle->open)
    return RC::BUFFERPOOL_CLOSED;
  *data = page_handle->frame->page.data;
  return RC::SUCCESS;
}

RC DiskBufferPool::mark_dirty(BPPageHandle *page_handle) {
  page_handle->frame->dirty = true;
  return RC::SUCCESS;
}

RC DiskBufferPool::unpin_page(BPPageHandle *page_handle) {
  page_handle->open = false;
 // LOG_INFO("unpin page :index:%d %s %d cpunt %d - 1",page_handle->frame - bp_manager_.getFrame(),page_handle->frame->fname.c_str(),page_handle->frame->page.page_num,page_handle->frame->pin_count);
  page_handle->frame->pin_count--;
 // bp_manager_.unpin_page(page_handle->frame);
  //LOG_INFO("unpin page fin:index:%d %s %d count %d",page_handle->frame - bp_manager_.getFrame(),page_handle->frame->fname.c_str(),page_handle->frame->page.page_num,page_handle->frame->pin_count);
 //bp_manager_.print();
  return RC::SUCCESS;
}

RC DiskBufferPool::unpin_page(const char *file_name, int page_num){
  bp_manager_.unpin_page(file_name, page_num);
  
  return RC::SUCCESS;
}

/**
 * dispose_page will delete the data of the page of pageNum
 * force_page will flush the page of pageNum
 * @param fileID
 * @param pageNum
 * @return
 */
RC DiskBufferPool::dispose_page(const char *file_name, PageNum page_num) {
  RC rc;
  BPFileHandle *file_handle = nullptr;
  rc = bp_manager_.open_file(file_name, &file_handle);
  if(rc != RC::SUCCESS)
    return rc;
  rc = bp_manager_.dispose_page(file_name, page_num);
  if(rc != RC::SUCCESS) {
    LOG_ERROR("Failed to dispose page, file :%s, rc:%s", file_name, strrc(rc));
    return rc;
  }

  file_handle->hdr_frame->dirty = true;
  file_handle->file_sub_header->allocated_pages--;
  char tmp = 1 << (page_num % 8);
  file_handle->bitmap[page_num / 8] &= ~tmp;
  return RC::SUCCESS;
}

/**
 *  will delete the data of the page of pageNum
 * force_page will flush the page of pageNum
 * @param fileHandle
 * @param pageNum
 * @return
 */
RC DiskBufferPool::force_page(const char *file_name, PageNum page_num) {
  RC rc = bp_manager_.force_page(file_name,page_num);
  if(rc != RC::SUCCESS)
    LOG_ERROR("Failed to flush page:%s:%d.", file_name, page_num);
  return rc;
}

RC DiskBufferPool::flush_all_pages(const char *file_name) {
  return force_all_pages(file_name);
}

RC DiskBufferPool::dispose_all_pages(const char *file_name){
 return bp_manager_.dispose_all_pages(file_name);
}

RC DiskBufferPool::force_all_pages(const char *file_name) {
  RC rc = bp_manager_.force_all_pages(file_name);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to flush all pages' of %s.", file_name);
      return rc;
  }
  return RC::SUCCESS;
}

RC BPManager::flush_block(Frame *frame) {
  // TODO
  // The better way is use mmap the block into memory,
  // so it is easier to flush data to file.
  if(!frame)
    return RC::SUCCESS;
  s64_t offset = ((s64_t)frame->page.page_num) * sizeof(Page);
  if (lseek(frame->file_desc, offset, SEEK_SET) == offset - 1) {
    LOG_ERROR("Failed to flush page %lld of %d due to failed to seek %s.",
              offset, frame->file_desc, strerror(errno));
    return RC::IOERR_SEEK;
  }
  
  if (write(frame->file_desc, &(frame->page), sizeof(Page)) != sizeof(Page)) {
    LOG_ERROR("Failed to flush page %lld of %d due to %s file:%s.", offset,
              frame->file_desc, strerror(errno),frame->fname.c_str());
   // print();
    return RC::IOERR_WRITE;
  }
  frame->dirty = false;
  LOG_DEBUG("Flush block. file desc=%d, page num=%d", frame->file_desc, frame->page.page_num);

  return RC::SUCCESS;
}

RC DiskBufferPool::allocate_block(Frame **buffer) {
  return bp_manager_.alloc(buffer);
}

RC DiskBufferPool::dispose_block(Frame *buf) {
  if (buf->pin_count != 0) {
    LOG_WARN("Begin to free page %d of %d, but it's pinned.", buf->page.page_num,
             buf->file_desc);
    return RC::LOCKED_UNLOCK;
  }
 // LOG_INFO("flush page:%d",buf->page.page_num);
  if (buf->dirty) {
    RC rc = bp_manager_.flush_block(buf);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  buf->dirty = false;
  bp_manager_.dispose_page(buf->file_desc,buf->page.page_num);
  LOG_DEBUG("dispost block frame =%p", buf);
  return RC::SUCCESS;
}

RC DiskBufferPool::get_page_count(const char *file_name, int *page_count) {
  RC rc = RC::SUCCESS;
  BPFileHandle *p = nullptr;
  rc = bp_manager_.open_file(file_name, &p);
  if(RC::SUCCESS != rc)
    return rc;
  *page_count = p->file_sub_header->page_count;
  return RC::SUCCESS;
}

RC DiskBufferPool::check_page_num(PageNum page_num, BPFileHandle *file_handle) {
  if (page_num >= file_handle->file_sub_header->page_count) {
    LOG_ERROR("Invalid pageNum:%d, file's name:%s", page_num,
              file_handle->file_name);
    return RC::BUFFERPOOL_INVALID_PAGE_NUM;
  }
  if ((file_handle->bitmap[page_num / 8] & (1 << (page_num % 8))) == 0) {
    LOG_ERROR("Invalid pageNum:%d, file's name:%s", page_num,
              file_handle->file_name);
    return RC::BUFFERPOOL_INVALID_PAGE_NUM;
  }
  return RC::SUCCESS;
}

RC BPManager::load_page(PageNum page_num, BPFileHandle *file_handle, Frame *frame) {
  s64_t offset = ((s64_t)page_num) * sizeof(Page);
  if (lseek(file_handle->file_desc, offset, SEEK_SET) == - 1) {
    LOG_ERROR("Failed to load page %s:%d, due to failed to lseek:%s.",
              file_handle->file_name, page_num, strerror(errno));

    return RC::IOERR_SEEK;
  }
  if (read(file_handle->file_desc, &(frame->page), sizeof(Page)) !=
      sizeof(Page)) {
    LOG_ERROR("Failed to load page %s:%d, due to failed to read data:%s.",
              file_handle->file_name, page_num, strerror(errno));
    return RC::IOERR_READ;
  }
  return RC::SUCCESS;
}

BPManager::BPManager(int size) {
  this->size = size;
  frame_ = (Frame*)malloc(sizeof(Frame)*size);
  items_ = (LruItem*)malloc(sizeof(LruItem)*(size+2));
  head_ = &items_[size];
  head_->val = -1;
  tail_ = &items_[size+1];
  tail_->val = -1;
  head_->next = tail_;
  head_->pre = nullptr;
  tail_->pre = head_;
  tail_->next = nullptr;
  for(int i = 0; i < size - 1; ++i){
    items_[i].val = i;
    items_[i].next = &items_[i+1];
  }
  items_[size - 1].val = size - 1;
  items_[size - 1].next = nullptr;
  free_list_ = &items_[0];
}

BPManager::~BPManager() {
  free(frame_);
  size = 0;
  frame_ = nullptr;
  free(items_);
  items_ = nullptr;
  head_ = nullptr;
  tail_ = nullptr;
  free_list_ = nullptr;
}

RC BPManager::alloc(Frame **buf) {
  LruItem *iter = nullptr;
  if(!free_list_){
      //evict
    bool found = false;
    for(LruItem *it = tail_->pre; it->val != -1; it = it->pre){
      //if(frame_[it->val].pin_count == 0){
      if(frame_[it->val].page.page_num != 0){
        iter = it;
        found = true;
        break;
      }else if(hash_map_[frame_[it->val].file_desc].size() == 1){
        //只剩第0页
        iter = it;
        found = true;
        break;
      }
    }
    
    if(!found){
    //  print();
      LOG_ERROR("All pages have been used and pinned.");
      return RC::NOMEM;
    }
    Frame *f = frame_ + iter->val;
    if(f->dirty){
      RC rc = flush_block(f);
      if (RC::SUCCESS != rc){
        return rc;
      }
    }
    f->dirty = false;
    if(hash_map_[f->file_desc].size() == 1){
      //关闭文件
      int i = file_map_[f->fname];
      if(open_list_[i])
        free(open_list_[i]);
      open_list_[i] = nullptr;
      close(f->file_desc);
      f->file_desc = -1;
      hash_map_.erase(f->file_desc);
      file_map_.erase(f->fname);
    }
    iter->pre->next = iter->next;
    iter->next->pre = iter->pre;
    hash_map_[f->file_desc].erase(f->page.page_num);
    if(hash_map_[f->file_desc].size()==0)
      hash_map_.erase(f->file_desc);
  }else{
    iter = free_list_;
    free_list_ = free_list_->next;
  }
  //print();
  if(iter){
    iter->pre = nullptr;
    iter->next = nullptr;
    *buf = frame_ + iter->val;
  }
  //LOG_INFO("iter->val:%d",iter->val);
  return RC::SUCCESS;
}

RC BPManager::open_file(const char *file_name, BPFileHandle **fhandle){
  std::string fname = file_name;
  if(file_map_.find(fname) != file_map_.end()){
    *fhandle = open_list_[file_map_[fname]];
    return RC::SUCCESS;
  }
  int i = 0;
  while (i < MAX_OPEN_FILE && open_list_[i]){
    i++;
  }
  RC rc = RC::SUCCESS;
  if(i >= MAX_OPEN_FILE){
    //evict
    Frame *f = nullptr;
    for(LruItem *it = tail_->pre; it->val != -1 && i >= MAX_OPEN_FILE; it = it->pre){
      f = frame_ + it->val;
      //f(frame_[it->val].pin_count == 0){
      if(frame_[it->val].page.page_num != 0){
        if(f->dirty){
          rc = flush_block(f);
          if (RC::SUCCESS != rc){
            return rc;
          }
        }
        f->dirty = false;
        it->pre->next = it->next;
        it->next->pre = it->pre;
        it->next = free_list_;
        free_list_ = it;
        hash_map_[f->file_desc].erase(f->page.page_num);
      }
      if(hash_map_[f->file_desc].size() <= 1){ //只剩第0页
        it = hash_map_[f->file_desc][0];
        Frame *f = frame_ + it->val;
        if(f->dirty){
          rc = flush_block(f);
          if (RC::SUCCESS != rc){
            return rc;
          }
        }
        f->dirty = false;
        it->pre->next = it->next;
        it->next->pre = it->pre;
        it->next = free_list_;
        free_list_ = it;
        hash_map_[f->file_desc].erase(0);
        i = file_map_[f->fname];
        file_map_.erase(f->fname);
        hash_map_.erase(f->file_desc);
        if(open_list_[i])
          free(open_list_[i]);
        open_list_[i] = nullptr;
        close(f->file_desc);
        f->file_desc = -1;
        break;
      }
    }
  }
  if (i >= MAX_OPEN_FILE) {
    LOG_ERROR("Failed to open file %s, because too much files has been opened.",
              file_name);
    return RC::BUFFERPOOL_OPEN_TOO_MANY_FILES;
  }

  int fd = 0;
  if ((fd = open(file_name, O_RDWR)) < 0) {
    LOG_ERROR("Failed to open file %s, because %s.", file_name, strerror(errno));
    return RC::IOERR_ACCESS;
  }
  LOG_INFO("Successfully open file %s.", file_name);

  BPFileHandle *file_handle = new (std::nothrow) BPFileHandle();
  if (file_handle == nullptr) {
    LOG_ERROR("Failed to alloc memory of BPFileHandle for %s.", file_name);
    close(fd);
    return RC::NOMEM;
  }

  RC tmp;
  file_handle->bopen = true;
  int file_name_len = strlen(file_name) + 1;
  char *cloned_file_name = new char [file_name_len];
  snprintf(cloned_file_name, file_name_len, "%s", file_name);
  file_handle->file_name = cloned_file_name;
  file_handle->file_desc= fd;
  if ((tmp = alloc(&file_handle->hdr_frame)) != RC::SUCCESS) {
    LOG_ERROR("Failed to allocate block for %s's BPFileHandle.", file_name);
    delete file_handle;
    close(fd);
    return tmp;
  }
  file_handle->hdr_frame->dirty = false;
  file_handle->hdr_frame->acc_time = current_time();
  file_handle->hdr_frame->file_desc = fd;
  file_handle->hdr_frame->pin_count = 1;
  file_handle->hdr_frame->fname = file_name;
  if ((tmp = load_page(0, file_handle, file_handle->hdr_frame)) != RC::SUCCESS) {
    file_handle->hdr_frame->pin_count = 0;
    dispose_block(file_handle->hdr_frame);
    close(fd);
    delete file_handle;
    return tmp;
  }

  file_handle->hdr_page = &(file_handle->hdr_frame->page);
  file_handle->bitmap = file_handle->hdr_page->data + BP_FILE_SUB_HDR_SIZE;
  file_handle->file_sub_header = (BPFileSubHeader *)file_handle->hdr_page->data;
  open_list_[i] = file_handle;
  insert_page(file_handle->hdr_frame);
  file_map_[file_name] = i;
  *fhandle = file_handle;
  return RC::SUCCESS;
}

Frame *BPManager::alloc() {
  Frame *buf;
  RC rc = alloc(&buf);
  if(RC::SUCCESS == rc)
    return buf;
  else
    return nullptr;// TODO for test
}

void BPManager::insert_page(Frame *f){
  LruItem *iter = items_ + (f-frame_);
  hash_map_[f->file_desc][f->page.page_num]=iter;
  iter->next = head_->next;
  iter->pre = head_;
  head_->next->pre = iter;
  head_->next = iter;
}

void BPManager::dispose_block(Frame *f){
  LruItem *iter = items_ + (f-frame_);
  iter->next = free_list_;
  free_list_ = iter;
}

RC BPManager::dispose_page(int desc, PageNum page_num){
  if(hash_map_[desc].count(page_num)){
    LruItem *it = hash_map_[desc][page_num];
    //if(frame_[it->val].pin_count != 0)
    //  return RC::BUFFERPOOL_PAGE_PINNED;
    hash_map_[desc].erase(page_num);
    if(hash_map_[desc].empty())
      hash_map_.erase(desc);
    it->next->pre = it->pre;
    it->pre->next = it->next;
    it->next = free_list_;
    free_list_ = it;
  }
  return RC::SUCCESS;
}

RC BPManager::dispose_page(const char *file_name, PageNum page_num){
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return RC::SUCCESS;
  int desc = open_list_[file_map_[fname]]->file_desc;
  return dispose_page(desc, page_num);
}

RC BPManager::dispose_all_pages(const char *file_name){
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return RC::SUCCESS;
  int desc = open_list_[file_map_[fname]]->file_desc;
  for (auto it = hash_map_[desc].begin(); it != hash_map_[desc].end(); ++it) {
    it->second->next->pre = it->second->pre;
    it->second->pre->next = it->second->next;
    it->second->next = free_list_;
    free_list_ = it->second;
  }
  hash_map_.erase(desc);
  if(open_list_[file_map_[fname]]){
    free(open_list_[file_map_[fname]]);
    open_list_[file_map_[fname]] = nullptr;
  }
  file_map_.erase(fname);
  close(desc);
  return RC::SUCCESS;
}

RC BPManager::force_page(const char *file_name, PageNum page_num){
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return RC::SUCCESS;
  int desc = open_list_[file_map_[fname]]->file_desc;
  if(hash_map_[desc].count(page_num)){
    LruItem *it = hash_map_[desc][page_num];
    int pos = it->val;
    if (frame_[pos].dirty) {
      RC rc = flush_block(frame_ + pos);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      frame_[pos].dirty = false;
    }
  }
  return RC::SUCCESS;
}

RC BPManager::force_all_pages(const char *file_name){
  int pos;
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return RC::SUCCESS;
  int desc = open_list_[file_map_[fname]]->file_desc;
  for (auto it = hash_map_[desc].begin(); it != hash_map_[desc].end(); ++it) {
    pos = it->second->val;
    if (frame_[pos].dirty) {
      RC rc = flush_block(frame_ + pos);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      frame_[pos].dirty = false;
    } 
  }
  return RC::SUCCESS;
}

RC BPManager::close_file(const char *file_name){
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return RC::SUCCESS;
  if(open_list_[file_map_[fname]]){
    free(open_list_[file_map_[fname]]);
    int desc = open_list_[file_map_[fname]]->file_desc;
    
    open_list_[file_map_[fname]] = nullptr;
    for(auto it = hash_map_[desc].begin(); it != hash_map_[desc].end(); ++it){
      LruItem *p = it->second;
      p->next->pre = p->pre;
      p->pre->next = p->next;
      p->next = free_list_;
      p->pre = nullptr;
      free_list_ = p;
    }
    hash_map_.erase(desc);
  }
  file_map_.erase(fname);
  return RC::SUCCESS;
}

Frame *BPManager::get(const char *file_name, PageNum page_num){
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return nullptr;
  int desc = open_list_[file_map_[fname]]->file_desc;
  if(hash_map_[desc].find(page_num) != hash_map_[desc].end()){
    return frame_ + hash_map_[desc][page_num]->val;
  }
  return nullptr;
}

void BPManager::unpin_page(const char *file_name, int page_num){
  std::string fname = file_name;
  if(file_map_.find(fname) == file_map_.end())
    return ;
  int desc = open_list_[file_map_[fname]]->file_desc;
  if(hash_map_[desc].find(page_num) != hash_map_[desc].end()){
    (frame_ + hash_map_[desc][page_num]->val)->pin_count--;
  }
 // LOG_INFO("unpin page fin:index:%d %s %d count %d",hash_map_[desc][page_num]->val,file_name,page_num,(frame_ + hash_map_[desc][page_num]->val)->pin_count);
 // print();
}

void BPManager::print(){
   /*   LOG_INFO("print cache pages:");
      LruItem *p = head_;
      int count = 0;
      while(p){
        count++;
        
        if(p->val != -1){
          Frame * f = frame_ + p->val;
          LOG_INFO("%d %d %d %s",p->val,f->page.page_num,f->pin_count,f->fname.c_str());
        }
        else
          LOG_INFO("-1");
        p = p->next;
        if(count %10 == 0)
          LOG_INFO("###");
          if(count > 70)
          break;
      }
     /* p = free_list_;
      LOG_INFO("free");
      while(p){
        count++;
        LOG_INFO("%d  ",p->val);
        p = p->next;
        if(count > 70)
          break;
      }*/
    //  LOG_INFO("end");
  }