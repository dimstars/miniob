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
#include "storage/common/bplus_tree.h"
#include "storage/default/disk_buffer_pool.h"
#include "rc.h"
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"

IndexNode * BplusTreeHandler::get_index_node(char *page_data) const {
  IndexNode *node = (IndexNode  *)(page_data + sizeof(IndexFileHeader));
  node->keys = (char *)node + sizeof(IndexNode);
  node->rids = (RID *)(node->keys + file_header_.order * file_header_.key_length);
  return node;
}

RC BplusTreeHandler::sync() {
  return disk_buffer_pool_->flush_all_pages(file_name_.c_str());
}
RC BplusTreeHandler::create(const char *file_name, const std::vector<const FieldMeta*> *field_meta, bool unique){
  BPPageHandle page_handle;
  IndexNode *root;
  char *pdata;
  RC rc;
  DiskBufferPool *disk_buffer_pool = theGlobalDiskBufferPool();
  rc = disk_buffer_pool->create_file(file_name);
  if(rc!=SUCCESS){
    return rc;
  }
  if(rc != SUCCESS){
    LOG_ERROR("Failed to open file. file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }
  rc = disk_buffer_pool->allocate_page(file_name, &page_handle);
  if(rc!=SUCCESS){
    LOG_ERROR("Failed to allocate page. file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }
  rc = disk_buffer_pool->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    LOG_ERROR("Failed to get data. file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }

  PageNum page_num;
  rc = disk_buffer_pool->get_page_num(&page_handle, &page_num);
  if(rc!=SUCCESS){
    LOG_ERROR("Failed to get page num. file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }
  
  IndexFileHeader *file_header =(IndexFileHeader *)pdata;
  int attr_length = RECORD_BITMAP;
  for(int i = 0; i < field_meta->size(); ++i){
    file_header->field_len[i] = (*field_meta)[i]->len();
    file_header->attr_types[i] = (*field_meta)[i]->type();    
    attr_length += (*field_meta)[i]->len();
  }
  
  file_header->node_num = 1;
  file_header->attr_length = attr_length;
  file_header->key_length = attr_length + sizeof(RID);
  file_header->order=((int)BP_PAGE_DATA_SIZE-sizeof(IndexFileHeader)-sizeof(IndexNode))/(attr_length+2*sizeof(RID));
  file_header->root_page = page_num;
  file_header->field_num = field_meta->size();
  file_header_.field_num = file_header->field_num;
  memcpy(&file_header_, pdata, sizeof(file_header_));
  root = get_index_node(pdata);
  root->is_leaf = 1;
  root->key_num = 0;
  root->parent = -1;
  root->keys = nullptr;
  root->rids = nullptr;

  rc = disk_buffer_pool->mark_dirty(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  disk_buffer_pool_ = disk_buffer_pool;
  file_name_ = file_name;
  unique_index_ = unique;
  header_dirty_ = false;
  return SUCCESS;
}

RC BplusTreeHandler::open(const char *file_name, bool unique) {
  BPPageHandle page_handle;
  char *pdata;
  if (disk_buffer_pool_ != nullptr) {
    return RC::RECORD_OPENNED;
  }
  RC rc = SUCCESS;
  DiskBufferPool *disk_buffer_pool = theGlobalDiskBufferPool();
  this->file_name_ = file_name;

  rc = disk_buffer_pool->get_this_page(file_name_.c_str(), 1, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  memcpy(&file_header_,pdata,sizeof(file_header_));

  header_dirty_ = false;
  disk_buffer_pool_ = disk_buffer_pool;
  unique_index_ = unique;

  rc = disk_buffer_pool->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  return SUCCESS;
}

RC BplusTreeHandler::close() {
  sync();
  disk_buffer_pool_->close_file(file_name_.c_str());
  disk_buffer_pool_ = nullptr;
  return RC::SUCCESS;
}

static int CmpRid(const RID *rid1, const RID *rid2) {
  if(rid1->page_num > rid2->page_num)
    return 1;
  if(rid1->page_num < rid2 ->page_num)
    return -1;
  if(rid1->slot_num > rid2->slot_num)
    return 1;
  if(rid1->slot_num < rid2->slot_num)
    return -1;
  return 0;
}
int CompareKey(const char *pdata, const char *pkey, AttrType data_type, AttrType key_type, int attr_length) { // TODO 简化
  int i1,i2;
  float f1,f2;
  const char *s1,*s2;
  float cmp = 0;
  switch(data_type){
    case INTS: {
      if(key_type == INTS) {
        i1 = *(int *) pdata;
        i2 = *(int *) pkey;
        cmp = i1 - i2;
      } else {
        f1 = (float)*(int *) pdata;
        f2 = *(float *) pkey;
        cmp = f1 - f2;
      }
    }
      break;
    case FLOATS: {
      if(key_type == FLOATS) {
        f1 = *(float *) pdata;
        f2 = *(float *) pkey;
        cmp = f1 - f2;
      } else {
        f1 = *(float *) pdata;
        f2 = (float)*(int *) pkey;
        cmp = f1 - f2;
      }
    }
      break;
    case CHARS: {
      s1 = pdata;
      s2 = pkey;
      cmp = strncmp(s1, s2, attr_length);
    }
      break;
    case DATES: {
      i1 = *(int*)pdata;
      i2 = *(int*)pkey;
      cmp = i1 - i2;
    }
      break;
    default:{
      LOG_PANIC("Unknown attr type: %d", data_type);
      return -2;//This means error happens
    }
  }
  if(cmp > 0)
    return 1;
  else if(cmp == 0)
    return 0;
  else
    return -1;
}

int CompareMutiKey(const AttrType *data_type, const AttrType *key_type, const int *field_len, int field_num, const char *pdata, const char *pkey){
  int result = 0;
  int offset = RECORD_BITMAP;
  long mask = 0x1;
  const long *bitmap_data = (const long *)pdata;
  const long *bitmap_key = (const long *)pkey;
  for(int i = 0; i < field_num && result == 0; ++i){
    if(0 == ((*bitmap_data) & mask)){
      if(0 == ((*bitmap_key) & mask)){
        continue;
      }
      else{
        result = -1;
      }
    }else if(0 == ((*bitmap_key) & mask)){
      result = 1;
    }else{
      result = CompareKey(pdata + offset, pkey + offset, key_type[i], data_type[i],  field_len[i]);
    }
    offset += field_len[i];
    mask <<= 1;
  }
  return result;
}

int CmpMutiKey(const AttrType* data_type, const AttrType* key_type, const int *field_len, int field_num, int attr_length, const char *pdata, const char *pkey){
  int result = CompareMutiKey(data_type, key_type, field_len, field_num, pdata, pkey);

  LOG_INFO("cmp result:%d",result);
  if(0 != result)
    return result;
  RID *rid1 = (RID *) (pdata + attr_length);
  RID *rid2 = (RID *) (pkey + attr_length);
  return CmpRid(rid1, rid2);
}

RC BplusTreeHandler::find_leaf(const char *pkey, const AttrType *ktype, PageNum *leaf_page){
  RC rc;
  BPPageHandle page_handle;
  IndexNode *node;
  char *pdata;
  int i,tmp;
  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), file_header_.root_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  node = get_index_node(pdata);
  while(0 == node->is_leaf){
    for(i = 0; i < node->key_num; i++){
      tmp = CmpMutiKey(ktype, file_header_.attr_types, file_header_.field_len, \
                        file_header_.field_num, file_header_.attr_length, pkey, node->keys + i * file_header_.key_length);
      if(tmp < 0)
        break;
    }
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), node->rids[i].page_num, &page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }
    node = get_index_node(pdata);
  }
  rc = disk_buffer_pool_->get_page_num(&page_handle, leaf_page);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  return SUCCESS;
}

RC BplusTreeHandler::insert_into_leaf(PageNum leaf_page, const char *pkey, const RID *rid)
{
  int i,insert_pos,tmp;
  BPPageHandle  page_handle;
  char *pdata;
  char *from,*to;
  IndexNode *node;
  RC rc;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &page_handle);
  if(rc != SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc != SUCCESS){
    return rc;
  }
  node = get_index_node(pdata);

  for(insert_pos = 0; insert_pos < node->key_num; insert_pos++){
      tmp=CmpMutiKey(file_header_.attr_types, file_header_.attr_types, file_header_.field_len, \
                      file_header_.field_num, file_header_.attr_length, pkey, node->keys + insert_pos * file_header_.key_length);
    if (tmp == 0) {
      disk_buffer_pool_->unpin_page(&page_handle);
      return RC::RECORD_DUPLICATE_KEY;
    }
    if(tmp < 0)
      break;
  }
  for(i = node->key_num; i > insert_pos; i--){
    from = node->keys+(i-1)*file_header_.key_length;
    to = node->keys+i*file_header_.key_length;
    memcpy(to, from, file_header_.key_length);
    memcpy(node->rids + i, node->rids + i-1, sizeof(RID));
  }
  memcpy(node->keys + insert_pos * file_header_.key_length, pkey, file_header_.key_length);
  memcpy(node->rids + insert_pos, rid, sizeof(RID));
  node->key_num++; //叶子结点增加一条记录
  rc = disk_buffer_pool_->mark_dirty(&page_handle);
  if(rc != SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc != SUCCESS){
    return rc;
  }
  return SUCCESS;
}

RC BplusTreeHandler::print() {
  IndexNode *node;
  RC rc;
  BPPageHandle page_handle;
  int i,j;
  char *pdata;
  int page_count;
  rc = disk_buffer_pool_->get_page_count(file_name_.c_str(), &page_count);
  if (rc != SUCCESS) {
    return rc;
  }
  for(i=1; i <= page_count; i++){
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), i, &page_handle);
    if(rc==RC::BUFFERPOOL_INVALID_PAGE_NUM)
      continue;
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }
    node = get_index_node(pdata);
    printf("page_num :%d %d\n",i,node->is_leaf);
    for(j=0;j<node->key_num&&j<6;j++){
      printf("keynum :%d rids:page_num :%d,slotnum :%d\n", node->key_num, node->rids[j].page_num, node->rids[j].slot_num);
    }
    printf("\n");
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
  }
  return rc;
}

RC BplusTreeHandler::insert_into_leaf_after_split(PageNum leaf_page, const char *pkey, const RID *rid) {
  RC rc;
  BPPageHandle  page_handle1,page_handle2;
  IndexNode *leaf,*new_node;
  PageNum new_page,parent_page;
  RID *temp_pointers,tmprid;
  char *temp_keys,*new_key;
  char *pdata;
  int insert_pos,split,i,j,tmp;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &page_handle1);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle1, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  leaf = get_index_node(pdata);

  //add a new node
  rc = disk_buffer_pool_->allocate_page(file_name_.c_str(), &page_handle2);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_data(&page_handle2, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_page_num(&page_handle2, &new_page);
  if(rc!=SUCCESS){
    return rc;
  }
  new_node = get_index_node(pdata);
  new_node->key_num = 0;
  new_node->is_leaf = 1;
  new_node->parent = leaf->parent;

  parent_page = leaf->parent;

  // print();

  temp_keys = (char *)malloc(file_header_.key_length*file_header_.order);
  if(temp_keys == nullptr){
    LOG_ERROR("Failed to alloc memory for temp key. size=%d", file_header_.key_length*file_header_.order);
    return RC::NOMEM;
  }

  temp_pointers = (RID *)malloc((file_header_.order+1)*sizeof(RID));
  if(temp_pointers == nullptr){
    LOG_ERROR("Failed to alloc memory for temp pointers. size=%d", (file_header_.order + 1)* sizeof(RID));
    free(temp_keys);
    return RC::NOMEM;
  }

  for(insert_pos=0;insert_pos<leaf->key_num;insert_pos++){
    //tmp=CmpKey(file_header_.attr_type, file_header_.attr_type, file_header_.attr_length, pkey, leaf->keys+insert_pos*file_header_.key_length);
    tmp=CmpMutiKey(file_header_.attr_types, file_header_.attr_types, file_header_.field_len, \
                    file_header_.field_num, file_header_.attr_length, pkey, leaf->keys + insert_pos * file_header_.key_length);
    if(tmp<0)
      break;
  }
  for(i=0,j=0;i<leaf->key_num;i++,j++){
    if(j==insert_pos)
      j++;
    memcpy(temp_keys+j*file_header_.key_length,leaf->keys+i*file_header_.key_length,file_header_.key_length);
    memcpy(temp_pointers+j,leaf->rids+i,sizeof(RID));
  }
  memcpy(temp_keys+insert_pos*file_header_.key_length,pkey,file_header_.key_length);
  memcpy(temp_pointers+insert_pos,rid,sizeof(RID));

  split=file_header_.order/2;

  for(i=0;i<split;i++){
    memcpy(leaf->keys+i*file_header_.key_length,temp_keys+i*file_header_.key_length,file_header_.key_length);
    memcpy(leaf->rids+i,temp_pointers+i,sizeof(RID));
  }
  leaf->key_num=split;

  for(i=split,j=0;i<file_header_.order;i++,j++){
    memcpy(new_node->keys+j*file_header_.key_length,temp_keys+i*file_header_.key_length,file_header_.key_length);
    memcpy(new_node->rids+j,temp_pointers+i,sizeof(RID));
    new_node->key_num++;
  }

  free(temp_pointers);
  free(temp_keys);

  memcpy(new_node->rids+file_header_.order-1,leaf->rids+file_header_.order-1,sizeof(RID));
  tmprid.page_num = new_page;
  tmprid.slot_num = -1;
  memcpy(leaf->rids+file_header_.order-1,&tmprid,sizeof(RID));

  new_key=(char *)malloc(file_header_.key_length);
  if(new_key == nullptr){
    LOG_ERROR("Failed to alloc memory for new key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  memcpy(new_key,new_node->keys,file_header_.key_length);

  rc = disk_buffer_pool_->mark_dirty(&page_handle1);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }

  rc = disk_buffer_pool_->unpin_page(&page_handle1);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }

  rc = disk_buffer_pool_->mark_dirty(&page_handle2);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle2);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }

  rc=insert_into_parent(parent_page,leaf_page,new_key,new_page); // TODO 插入失败，应该回滚之前的叶子节点
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  free(new_key);
  return SUCCESS;
}

RC BplusTreeHandler::insert_intern_node(PageNum parent_page,PageNum left_page,PageNum right_page,const char *pkey) {
  int i,insert_pos;
  BPPageHandle page_handle;
  char *pdata;
  IndexNode *node;
  RID rid;
  RC rc;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), parent_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  node = get_index_node(pdata);

  insert_pos=0;
  while((insert_pos<=node->key_num)&&(node->rids[insert_pos].page_num != left_page))
    insert_pos++;
  for(i=node->key_num;i>insert_pos;i--){
    memcpy(node->rids+i+1,node->rids+i,sizeof(RID));
    memcpy(node->keys+i*file_header_.key_length,node->keys+(i-1)*file_header_.key_length,file_header_.key_length);
  }
  rid.page_num = right_page;
  rid.slot_num = BP_INVALID_PAGE_NUM; // TODO change to invalid page num
  memcpy(node->rids+insert_pos+1,&rid,sizeof(RID));
  memcpy(node->keys+insert_pos*file_header_.key_length,pkey,file_header_.key_length);
  node->key_num++;
  rc = disk_buffer_pool_->mark_dirty(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  return SUCCESS;
}

RC BplusTreeHandler::insert_intern_node_after_split(PageNum inter_page,PageNum left_page,PageNum right_page,const char *pkey) {
  RC rc;
  BPPageHandle page_handle1,page_handle2,child_page_handle;
  IndexNode *inter_node,*new_node,*child_node;
  PageNum new_page,child_page,parent_page;
  RID *temp_pointers,tmprid;
  char *temp_keys,*new_key;
  char *pdata;
  int insert_pos,i,j,split;
  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), inter_page, &page_handle1);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle1, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  inter_node = get_index_node(pdata);

  //add a new node
  rc = disk_buffer_pool_->allocate_page(file_name_.c_str(), &page_handle2);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle2, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_page_num(&page_handle2, &new_page);
  if(rc!=SUCCESS){
    return rc;
  }

  new_node = get_index_node(pdata);
  new_node->key_num=0;
  new_node->is_leaf=false;
  new_node->parent=inter_node->parent;

  parent_page=inter_node->parent;

  // print();

  temp_keys=(char *)malloc(file_header_.key_length*file_header_.order);
  if(temp_keys == nullptr){
    LOG_ERROR("Failed to alloc memory for temp keys. size=%d", 
              file_header_.key_length * file_header_.order);
    return RC::NOMEM;
  }

  temp_pointers = (RID *)malloc((file_header_.order + 1) * sizeof(RID));
  if(temp_pointers == nullptr){
    LOG_ERROR("Failed to alloc memory for temp pointers. size=%d", 
              (file_header_.order + 1) * sizeof(RID));
    free(temp_keys);
    return RC::NOMEM;
  }

  new_key=(char *)malloc(file_header_.key_length);
  if(new_key==nullptr){
    LOG_ERROR("Failed to alloc memory for new key. size=%d", file_header_.key_length);
    free(temp_keys);
    free(temp_pointers);
    return RC::NOMEM;
  }

  insert_pos=0;
  while((insert_pos<=inter_node->key_num)&&(inter_node->rids[insert_pos].page_num != left_page))
    insert_pos++;
  for(i=0,j=0;i<inter_node->key_num+1;i++,j++){
    if(j==insert_pos+1)
      j++;
    memcpy(temp_pointers+j,inter_node->rids+i,sizeof(RID));
  }
  for(i=0,j=0;i<inter_node->key_num;i++,j++){
    if(j==insert_pos)
      j++;
    memcpy(temp_keys+j*file_header_.key_length,inter_node->keys+i*file_header_.key_length,file_header_.key_length);
  }
  tmprid.page_num = right_page;
  tmprid.slot_num = -1;
  memcpy(temp_keys+insert_pos*file_header_.key_length,pkey,file_header_.key_length);
  memcpy(temp_pointers+insert_pos+1,&tmprid,sizeof(RID));

  split=(file_header_.order+1)/2;

  for(i=0;i<split-1;i++){
    memcpy(inter_node->keys+i*file_header_.key_length,temp_keys+i*file_header_.key_length,file_header_.key_length);
    memcpy(inter_node->rids+i,temp_pointers+i,sizeof(RID));
  }
  inter_node->key_num=split-1;
  memcpy(inter_node->rids+i,temp_pointers+i,sizeof(RID));
  memcpy(new_key,temp_keys+i*file_header_.key_length,file_header_.key_length);

  for(++i,j=0;i<file_header_.order;i++,j++){
    memcpy(new_node->keys+j*file_header_.key_length,temp_keys+i*file_header_.key_length,file_header_.key_length);
    memcpy(new_node->rids+j,temp_pointers+i,sizeof(RID));
    new_node->key_num++;
  }
  memcpy(new_node->rids+j,temp_pointers+i,sizeof(RID));

  free(temp_keys);
  free(temp_pointers);

  for(i=0;i<=new_node->key_num;i++){
    child_page=new_node->rids[i].page_num;
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), child_page, &child_page_handle);
    if(rc!=SUCCESS){
      free(new_key);
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&child_page_handle, &pdata);
    if(rc!=SUCCESS){
      free(new_key);
      return rc;
    }
    child_node=(IndexNode *)(pdata+sizeof(IndexFileHeader));
    child_node->parent=new_page;
    rc = disk_buffer_pool_->mark_dirty(&child_page_handle);
    if(rc!=SUCCESS){
      free(new_key);
      return rc;
    }
    rc = disk_buffer_pool_->unpin_page(&child_page_handle);
    if(rc!=SUCCESS){
      free(new_key);
      return rc;
    }
  }

  rc = disk_buffer_pool_->mark_dirty(&page_handle1);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle1);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  rc = disk_buffer_pool_->mark_dirty(&page_handle2);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle2);
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  // print();

  rc=insert_into_parent(parent_page,inter_page,new_key,new_page);

  // print();
  if(rc!=SUCCESS){
    free(new_key);
    return rc;
  }
  free(new_key);
  return SUCCESS;
}

RC BplusTreeHandler::insert_into_parent(PageNum parent_page, PageNum left_page, const char *pkey,PageNum right_page) {
  RC rc;
  BPPageHandle page_handle;
  IndexNode *node;
  char *pdata;
  if(parent_page==-1){
    return insert_into_new_root(left_page,pkey,right_page);
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), parent_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  node=(IndexNode *)(pdata+sizeof(IndexFileHeader));
  if(node->key_num<file_header_.order-1){
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    return  insert_intern_node(parent_page,left_page,right_page,pkey);
  }
  else{
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    return insert_intern_node_after_split(parent_page,left_page,right_page,pkey);
  }
}

RC BplusTreeHandler::insert_into_new_root(PageNum left_page, const char *pkey, PageNum right_page) {
  RC rc;
  BPPageHandle page_handle;
  IndexNode *root,*left,*right;
  PageNum root_page;
  RID rid;
  char *pdata;
  rc = disk_buffer_pool_->allocate_page(file_name_.c_str(), &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_page_num(&page_handle, &root_page);
  if(rc!=SUCCESS){
    return rc;
  }

  root = get_index_node(pdata);
  root->is_leaf=false;
  root->key_num=1;
  root->parent=-1;
  memcpy(root->keys,pkey,file_header_.key_length);
  rid.page_num = left_page;
  rid.slot_num = -1;
  memcpy(root->rids,&rid,sizeof(RID));
  rid.page_num = right_page;
  rid.slot_num = -1;
  memcpy(root->rids+1,&rid,sizeof(RID));

  rc = disk_buffer_pool_->mark_dirty(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), left_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  left=(IndexNode *)(pdata+sizeof(IndexFileHeader));
  left->parent=root_page;

  rc = disk_buffer_pool_->mark_dirty(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), right_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  right=(IndexNode *)(pdata+sizeof(IndexFileHeader));
  right->parent=root_page;

  rc = disk_buffer_pool_->mark_dirty(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  file_header_.root_page=root_page;
  header_dirty_ = true;
  return SUCCESS;
}

RC BplusTreeHandler::insert_entry(std::vector<const char *> *pkey, const RID *rid) {
  RC rc;
  PageNum leaf_page;
  BPPageHandle page_handle;
  char *pdata,*key;
  IndexNode *leaf;
  if(nullptr == disk_buffer_pool_){
    return RC::RECORD_CLOSED;
  }
  if(pkey->size() > file_header_.field_num){
    return RC::INVALID_ARGUMENT;
  }
  key=(char *)malloc(sizeof(char)*file_header_.key_length);
  if(key == nullptr){
    LOG_ERROR("Failed to alloc memory for key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  memset(key,0,RECORD_BITMAP);
  long mask = 0x1;
  long *map = (long*)key;
  int offset = RECORD_BITMAP;
  for(int i = 0; i < pkey->size(); ++i){
    if(pkey->at(i)){
      memcpy(key + offset, pkey->at(i), file_header_.field_len[i]);
      *map |= mask;
    }
    offset += file_header_.field_len[i];
    mask <<= 1;
    //TODO:支持null
  }
  memcpy(key + file_header_.attr_length, rid, sizeof(*rid));
  rc= find_leaf(key, file_header_.attr_types, &leaf_page);
  if(rc!=SUCCESS){
    free(key);
    return rc;
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &page_handle);
  if(rc!=SUCCESS){
    free(key);
    return rc;
  }

  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    free(key);
    return rc;
  }
  leaf=(IndexNode *)(pdata+sizeof(IndexFileHeader));

  if(unique_index_){
    long bitmap = *(long*)key;
    //允许多个NULL
    if(bitmap != 0){
      IndexNode *node = get_index_node(pdata);
      int tmp;
      for(int insert_pos = 0; insert_pos < node->key_num; insert_pos++){
        //TODO:允许多个NULL值
        // 对于隐式转换而言，此处pkey一定是已经经过隐式转换的数据
        tmp = CompareMutiKey(file_header_.attr_types,file_header_.attr_types,file_header_.field_len,file_header_.field_num,key,node->keys+insert_pos*file_header_.key_length);
        if (tmp == 0) {
          return RC::RECORD_DUPLICATE_KEY;
        }
        if(tmp < 0)
          break;
      }
    }
  }

  if(leaf->key_num<file_header_.order-1){
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      free(key);
      return rc;
    }
    rc=insert_into_leaf(leaf_page,key,rid);
    if(rc!=SUCCESS){
      free(key);
      return rc;
    }
    free(key);
    return SUCCESS;
  }
  else{
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      free(key);
      return rc;
    }

    // print();

    rc=insert_into_leaf_after_split(leaf_page,key,rid);
    if(rc!=SUCCESS){
      free(key);
    }
    free(key);
    return SUCCESS;
  }
}

RC BplusTreeHandler::get_entry(std::vector<const char *> *pkey,RID *rid) {
  RC rc;
  PageNum leaf_page;
  BPPageHandle page_handle;
  int i;
  char *pdata,*key;
  IndexNode *leaf;

  key=(char *)malloc(file_header_.key_length);
  if(key == nullptr){
    LOG_ERROR("Failed to alloc memory for key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  memset(key,0,RECORD_BITMAP);
  long mask = 0x1;
  long *map = (long*)key;
  int offset = RECORD_BITMAP;
  for(int i = 0; i < file_header_.field_num; ++i){
    memcpy(key + offset, (*pkey)[i], file_header_.field_len[i]);
    offset += file_header_.field_len[i];
    //TODO:支持null
    *map |= mask;
    mask <<= 1;
  }
  memcpy(key+file_header_.attr_length,rid,sizeof(RID));

  rc=find_leaf(key, file_header_.attr_types, &leaf_page);
  if(rc!=SUCCESS){
    free(key);
    return rc;
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &page_handle);
  if(rc!=SUCCESS){
    free(key);
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    free(key);
    return rc;
  }

  leaf = get_index_node(pdata);
  for(i=0;i<leaf->key_num;i++){
    if(0==CmpMutiKey(file_header_.attr_types, file_header_.attr_types, file_header_.field_len, \
                        file_header_.field_num, file_header_.attr_length, key, leaf->keys + i * file_header_.key_length)) {
      memcpy(rid,leaf->rids+i,sizeof(RID));
      free(key);
      return SUCCESS;
    }
  }
  free(key);
  return RC::RECORD_INVALID_KEY;
}

RC BplusTreeHandler::delete_entry_from_node(PageNum node_page,const char *pkey) {
  BPPageHandle page_handle;
  IndexNode *node;
  char *pdata;
  int delete_index,i,tmp;
  RC rc;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), node_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  node = get_index_node(pdata);

  for(delete_index=0;delete_index<node->key_num;delete_index++){
   // tmp=CmpKey(file_header_.attr_type, file_header_.attr_type, file_header_.attr_length, pkey, node->keys+delete_index*file_header_.key_length);
    tmp=CmpMutiKey(file_header_.attr_types, file_header_.attr_types, file_header_.field_len, \
                        file_header_.field_num, file_header_.attr_length, pkey, node->keys + delete_index * file_header_.key_length);
    if(tmp==0)
      break;
  }
  if(delete_index>=node->key_num){
    return RC::RECORD_INVALID_KEY;
  }
  i=delete_index;
  while(i<(node->key_num-1)){
    memcpy(node->keys+i*file_header_.key_length,node->keys+(i+1)*file_header_.key_length,file_header_.key_length);
    i++;
  }

  if(node->is_leaf)
    for(i=delete_index;i<(node->key_num-1);i++)
      memcpy(node->rids+i,node->rids+i+1,sizeof(RID));
  else
    for(i=delete_index+1;i<node->key_num;i++)
      memcpy(node->rids+i,node->rids+i+1,sizeof(RID));
  node->key_num--;

  rc = disk_buffer_pool_->mark_dirty(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  return SUCCESS;
}

RC BplusTreeHandler::coalesce_node(PageNum leaf_page,PageNum right_page)
{
  BPPageHandle left_handle,right_handle,parent_handle,tmphandle;
  IndexNode *left,*right,*parent,*node;
  char *pdata,*tmp_key;
  PageNum parent_page;
  RC rc;
  int i,j,k,start;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &left_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&left_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  left = get_index_node(pdata);

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), right_page, &right_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_data(&right_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  right = get_index_node(pdata);

  parent_page=left->parent;
  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), parent_page, &parent_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_data(&parent_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  parent = get_index_node(pdata);

  for(k=0;k<parent->key_num;k++)
    if((parent->rids[k].page_num) == leaf_page)
      break;

  start=left->key_num;
  if(left->is_leaf==false){
    memcpy(left->keys+start*file_header_.key_length,parent->keys+k*file_header_.key_length,file_header_.key_length);
    start++;
    left->key_num++;
  }
  for(i=start,j=0;j<right->key_num;i++,j++){
    memcpy(left->keys+i*file_header_.key_length,right->keys+j*file_header_.key_length,file_header_.key_length);
    memcpy(left->rids+i,right->rids+j,sizeof(RID));
    left->key_num++;
  }

  if(left->is_leaf)
    memcpy(left->rids+file_header_.order-1,right->rids+file_header_.order-1,sizeof(RID));
  else{
    memcpy(left->rids+i,right->rids+j,sizeof(RID));

    for(i=start;i<=left->key_num;i++){
      rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), left->rids[i].page_num, &tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->get_data(&tmphandle, &pdata);
      if(rc!=SUCCESS){
        return rc;
      }
      node=(IndexNode *)(pdata+sizeof(IndexFileHeader));
      node->parent=leaf_page;

      rc = disk_buffer_pool_->mark_dirty(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
    }
  }

  tmp_key=(char *)malloc(file_header_.key_length);
  if(tmp_key== nullptr){
    LOG_ERROR("Failed to alloc memory for key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  memcpy(tmp_key,parent->keys+k*file_header_.key_length,file_header_.key_length);

  rc = disk_buffer_pool_->mark_dirty(&left_handle);
  if(rc!=SUCCESS){
    free(tmp_key);
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&left_handle);
  if(rc!=SUCCESS){
    free(tmp_key);
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&right_handle);
  if(rc!=SUCCESS){
    free(tmp_key);
    return rc;
  }
  rc = disk_buffer_pool_->dispose_page(file_name_.c_str(), right_page);
  if(rc!=SUCCESS){
    free(tmp_key);
    return rc;
  }

  rc = disk_buffer_pool_->unpin_page(&parent_handle);
  if(rc!=SUCCESS){
    free(tmp_key);
    return rc;
  }

  rc= delete_entry_internal(parent_page,tmp_key);
  if(rc!=SUCCESS){
    free(tmp_key);
    return rc;
  }
  free(tmp_key);
  return SUCCESS;
}

RC BplusTreeHandler::redistribute_nodes(PageNum leaf_page,PageNum right_page)
{
  BPPageHandle left_handle,right_handle,parent_handle,tmphandle;
  IndexNode *left,*right,*parent,*node;
  char *pdata;
  PageNum parent_page;
  RC rc;
  int min_key,i,k;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &left_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&left_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  left = get_index_node(pdata);

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), right_page, &right_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&right_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  right = get_index_node(pdata);

  parent_page=left->parent;
  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), parent_page, &parent_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&parent_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  parent = get_index_node(pdata);

  for(k=0;k<parent->key_num;k++)
    if(parent->rids[k].page_num == leaf_page)
      break;
  if(left->is_leaf){
    min_key=file_header_.order/2;
    if(left->key_num<min_key){
      memcpy(left->keys+left->key_num*file_header_.key_length,right->keys,file_header_.key_length);
      memcpy(left->rids+left->key_num,right->rids,sizeof(RID));
      left->key_num++;

      for(i=0;i<right->key_num-1;i++){
        memcpy(right->keys+i*file_header_.key_length,right->keys+(i+1)*file_header_.key_length,file_header_.key_length);
        memcpy(right->rids+i,right->rids+i+1,sizeof(RID));
      }
      right->key_num--;
      memcpy(parent->keys+k*file_header_.key_length,right->keys,file_header_.key_length);
    }
    else{
      for(i=right->key_num;i>0;i--){
        memcpy(right->keys+i*file_header_.key_length,right->keys+(i-1)*file_header_.key_length,file_header_.key_length);
        memcpy(right->rids+i,right->rids+i-1,sizeof(RID));
      }
      memcpy(right->keys,left->keys+(left->key_num-1)*file_header_.key_length,file_header_.key_length);

      left->key_num--;
      right->key_num++;
      memcpy(parent->keys+k*file_header_.key_length,right->keys,file_header_.key_length);
    }
  }
  else{
    min_key=(file_header_.order+1)/2-1;
    if(left->key_num<min_key){
      memcpy(left->keys+left->key_num*file_header_.key_length,parent->keys+k*file_header_.key_length,file_header_.key_length);
      memcpy(left->rids+left->key_num+1,right->rids,sizeof(RID));
      left->key_num++;

      memcpy(parent->keys+k*file_header_.key_length,right->keys,file_header_.key_length);
      for(i=0;i<right->key_num-1;i++){
        memcpy(right->keys+i*file_header_.key_length,right->keys+(i+1)*file_header_.key_length,file_header_.key_length);
        memcpy(right->rids+i,right->rids+i+1,sizeof(RID));
      }
      right->key_num--;

      rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), left->rids[left->key_num].page_num,&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->get_data(&tmphandle, &pdata);
      if(rc!=SUCCESS){
        return rc;
      }
      node=(IndexNode *)(pdata+sizeof(IndexFileHeader));
      node->parent=leaf_page;
      rc = disk_buffer_pool_->mark_dirty(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
    }
    else{
      for(i=right->key_num;i>0;i--){
        memcpy(right->keys+i*file_header_.key_length,right->keys+(i-1)*file_header_.key_length,file_header_.key_length);
        memcpy(right->rids+i,right->rids+i-1,sizeof(RID));
      }
      memcpy(right->keys,parent->keys+k*file_header_.key_length,file_header_.key_length);
      memcpy(right->rids,left->rids+left->key_num,sizeof(RID));

      right->key_num++;
      memcpy(parent->keys+k*file_header_.key_length,left->keys+(left->key_num-1)*file_header_.key_length,file_header_.key_length);
      left->key_num--;

      rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), right->rids[0].page_num, &tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->get_data(&tmphandle, &pdata);
      if(rc!=SUCCESS){
        return rc;
      }
      node = get_index_node(pdata);
      node->parent=right_page;
      rc = disk_buffer_pool_->mark_dirty(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
    }
  }
  rc = disk_buffer_pool_->mark_dirty(&left_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&left_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->mark_dirty(&right_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->unpin_page(&right_handle);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->mark_dirty(&parent_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&parent_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  return SUCCESS;
}

RC BplusTreeHandler::delete_entry_internal(PageNum page_num,const char *pkey) {
  BPPageHandle parent_handle,page_handle,left_handle,right_handle,tmphandle;
  IndexNode *node,*parent,*left,*right,*tmpnode;
  PageNum leaf_page,right_page;
  char *pdata;
  RC rc;
  int delete_index,min_key;

  rc=delete_entry_from_node(page_num,pkey);
  if(rc!=SUCCESS){
    return rc;
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), page_num, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }
  node = get_index_node(pdata);

  if(node->parent==-1){
    if(node->key_num==0&&node->is_leaf==false){
      rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), node->rids[0].page_num, &tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->get_data(&tmphandle, &pdata);
      if(rc!=SUCCESS){
        return rc;
      }
      tmpnode=(IndexNode *)(pdata+sizeof(IndexFileHeader));
      tmpnode->parent=-1;
      rc = disk_buffer_pool_->mark_dirty(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&tmphandle);
      if(rc!=SUCCESS){
        return rc;
      }

      file_header_.root_page=node->rids[0].page_num;
      header_dirty_ = true;

      rc = disk_buffer_pool_->unpin_page(&page_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->dispose_page(file_name_.c_str(), page_num);
      if(rc!=SUCCESS){
        return rc;
      }
      return SUCCESS;
    }

    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    return SUCCESS;
  }

  if(node->is_leaf)
    min_key=file_header_.order/2;
  else
    min_key=(file_header_.order+1)/2-1;

  if(node->key_num>=min_key){
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    return SUCCESS;
  }

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), node->parent, &parent_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&parent_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  parent = get_index_node(pdata);

  delete_index=0;
  while(delete_index<=parent->key_num){
    if((parent->rids[delete_index].page_num) == page_num)
      break;
    delete_index++;
  }

  if(delete_index==0){
    leaf_page=page_num;
    right_page=parent->rids[delete_index+1].page_num;
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), right_page, &right_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&right_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }
    right=(IndexNode *)(pdata+sizeof(IndexFileHeader));

    if(right->key_num>min_key){
      rc = disk_buffer_pool_->unpin_page(&page_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&parent_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&right_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      return redistribute_nodes(page_num,right_page);
    }
    else{
      rc = disk_buffer_pool_->unpin_page(&page_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&parent_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&right_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      return coalesce_node(page_num,right_page);
    }
  }
  else{
    leaf_page=parent->rids[delete_index-1].page_num;
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), leaf_page, &left_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&left_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }
    left=(IndexNode *)(pdata+sizeof(IndexFileHeader));

    if(left->key_num>min_key){
      rc = disk_buffer_pool_->unpin_page(&page_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&parent_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&left_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      return redistribute_nodes(leaf_page,page_num);
    }
    else{
      rc = disk_buffer_pool_->unpin_page(&page_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&parent_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      rc = disk_buffer_pool_->unpin_page(&left_handle);
      if(rc!=SUCCESS){
        return rc;
      }
      return coalesce_node(leaf_page,page_num);
    }
  }
}

RC BplusTreeHandler::delete_entry(std::vector<const char *> *data, const RID *rid) {
  RC rc;
  PageNum leaf_page;
  char *pkey;
  pkey=(char *)malloc(file_header_.key_length);
  if(nullptr == pkey){
    LOG_ERROR("Failed to alloc memory for key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  memset(pkey,0,RECORD_BITMAP);
  long mask = 0x1;
  long *map = (long*)pkey;
  int offset = RECORD_BITMAP;
  for(int i = 0; i < file_header_.field_num; ++i){
    if(data->at(i)){
      memcpy(pkey + offset, data->at(i), file_header_.field_len[i]);
      *map |= mask;
    }
    offset += file_header_.field_len[i];
    //TODO:支持null
    mask <<= 1;
  }
  memcpy(pkey + file_header_.attr_length, rid ,sizeof(*rid));

  rc=find_leaf(pkey, file_header_.attr_types, &leaf_page);
  if(rc!=SUCCESS){
    free(pkey);
    return rc;
  }
  rc=delete_entry_internal(leaf_page,pkey);
  free(pkey);
  return rc;
}


RC BplusTreeHandler::print_tree() {
  BPPageHandle page_handle;
  IndexNode *node;
  PageNum page_num;
  char *pdata,*pkey;
  int i;
  RC rc;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), file_header_.root_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  node = get_index_node(pdata);

  while(!node->is_leaf){
    page_num=node->rids[0].page_num;
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), page_num, &page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }
    node=(IndexNode *)(pdata+sizeof(IndexFileHeader));
    node->keys=(char *)node+sizeof(IndexNode);
    node->rids=(RID *)(node->keys+file_header_.order*file_header_.key_length);
  }
  page_num=1;
  while(page_num!=0){
    for(i=0;i<node->key_num;i++){
      pkey=node->keys+i*file_header_.key_length;
      printf("key : %d,rids (page_num:%d slotnum %d)\n",*(int *)pkey,node->rids[i].page_num,node->rids[i].slot_num);
    }
    printf("next node:%d\n",page_num);
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    page_num=node->rids[file_header_.order-1].page_num;
    if(page_num==0)
      break;
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), page_num, &page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }

    node = get_index_node(pdata);
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  return SUCCESS;
}
RC BplusTreeHandler::find_first_index_satisfied(CompOp compop, const char *key, const AttrType* ktype, int attr_num, PageNum *page_num, int *rididx) {
  BPPageHandle page_handle;
  IndexNode *node;
  PageNum leaf_page,next;
  char *pdata,*pkey;
  RC rc;
  int i,tmp;
  RID rid;
  if(compop == LESS_THAN || compop == LESS_EQUAL || compop == NOT_EQUAL){
    rc = get_first_leaf_page(page_num);
    if(rc != SUCCESS){
      return rc;
    }
    *rididx=0;
    return SUCCESS;
  }
  rid.page_num = -1;
  rid.slot_num = -1;
  pkey=(char *)malloc(file_header_.attr_length + sizeof(RID));
  if(pkey == nullptr){
    LOG_ERROR("Failed to alloc memory for key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  memcpy(pkey, key, file_header_.attr_length);
  memcpy(pkey + file_header_.attr_length, &rid, sizeof(RID));

  rc = find_leaf(pkey, ktype, &leaf_page);
  if(rc != SUCCESS){
    free(pkey);
    return rc;
  }

  next=leaf_page;

  while(next > 0){
    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), next, &page_handle);
    if(rc!=SUCCESS){
      break;
    }
    rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
    if(rc!=SUCCESS){
      break;
    }

    node = get_index_node(pdata);
    for(i = 0; i < node->key_num; i++){
      tmp=CompareMutiKey(file_header_.attr_types,ktype,file_header_.field_len,attr_num,node->keys+i*file_header_.key_length,pkey);
      if(compop == EQUAL_TO ||compop == GREAT_EQUAL){
        if(tmp>=0){
          rc = disk_buffer_pool_->get_page_num(&page_handle, page_num);
          if(rc == SUCCESS){
            *rididx=i;
            rc = disk_buffer_pool_->unpin_page(&page_handle);
          }
          free(pkey);
          return rc;
        }
      }
      if(compop == GREAT_THAN){
        if(tmp > 0){
          rc = disk_buffer_pool_->get_page_num(&page_handle, page_num);
          if(rc == SUCCESS){
            *rididx=i;
            rc = disk_buffer_pool_->unpin_page(&page_handle);
          }
          free(pkey);
          return rc;
        }
      }
    }
    next=node->rids[file_header_.order-1].page_num;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc != SUCCESS){
    free(pkey);
    return rc;
  }
  free(pkey);
  return RC::RECORD_EOF;
}

RC BplusTreeHandler::get_first_leaf_page(PageNum *leaf_page) {
  RC rc;
  BPPageHandle page_handle;
  PageNum page_num;
  IndexNode *node;
  char *pdata;

  rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), file_header_.root_page, &page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
  if(rc!=SUCCESS){
    return rc;
  }

  node = get_index_node(pdata);

  while(node->is_leaf==false){
    page_num=node->rids[0].page_num;
    rc = disk_buffer_pool_->unpin_page(&page_handle);
    if(rc!=SUCCESS){
      return rc;
    }

    rc = disk_buffer_pool_->get_this_page(file_name_.c_str(), page_num, &page_handle);
    if(rc!=SUCCESS){
      return rc;
    }
    rc = disk_buffer_pool_->get_data(&page_handle, &pdata);
    if(rc!=SUCCESS){
      return rc;
    }

    node = get_index_node(pdata);
  }
  rc = disk_buffer_pool_->get_page_num(&page_handle, leaf_page);
  if(rc!=SUCCESS){
    return rc;
  }
  rc = disk_buffer_pool_->unpin_page(&page_handle);
  if(rc!=SUCCESS){
    return rc;
  }
  return SUCCESS;
}

BplusTreeScanner::BplusTreeScanner(BplusTreeHandler &index_handler) : index_handler_(index_handler){
}
RC BplusTreeScanner::open(CompOp comp_op, std::vector<const char *> *values, std::vector<AttrType> *type){
  RC rc;
  if(opened_){
    return RC::RECORD_OPENNED;
  }
  
  char *value_copy = (char *)malloc(index_handler_.file_header_.attr_length);
  AttrType *value_types = (AttrType *)malloc(sizeof(AttrType) * type->size());
  if(value_copy == nullptr || value_types == nullptr){
    LOG_ERROR("Failed to alloc memory for value. size=%d", index_handler_.file_header_.attr_length);
    return RC::NOMEM;
  }
  memset(value_copy,0,RECORD_BITMAP);
  int offset = RECORD_BITMAP;
  long *pbitmap = (long *)value_copy;
  long mask = 0x1;
  for(int i = 0; i < values->size(); ++i){
    if(type->at(i) != NULLS){
      memcpy(value_copy + offset, values->at(i), index_handler_.file_header_.field_len[i]);
      *pbitmap |= mask;
    }
    value_types[i] = type->at(i);
    mask <<= 1;
    offset += index_handler_.file_header_.field_len[i];
  }
  value_ = value_copy;
  type_ = value_types;
  attr_num_ = values->size();
  comp_op_ = comp_op;
  rc = index_handler_.find_first_index_satisfied(comp_op, value_copy, value_types, attr_num_, &next_page_num_, &index_in_node_);
  if(rc != SUCCESS){
    if(rc == RC::RECORD_EOF){
      next_page_num_ = -1;
      index_in_node_ = -1;
    }
    else
      return rc;
  }
  num_fixed_pages_ = 1;
  next_index_of_page_handle_=0;
  pinned_page_count_ = 0;
  opened_ = true;
  return SUCCESS;
}

RC BplusTreeScanner::close() {
  if (!opened_) {
    return RC::RECORD_SCANCLOSED;
  }
  // TODO
  free((void *)value_);
  value_ = nullptr;
  free((void *)type_);
  type_ = nullptr;
  opened_ = false;
  return RC::SUCCESS;
}

RC BplusTreeScanner::next_entry(RID *rid) {
  RC rc;
  if(!opened_){
    return RC::RECORD_CLOSED;
  }
  LOG_INFO("rid %d %d",rid->page_num,rid->slot_num);
  rc = get_next_idx_in_memory(rid);//和RM中一样，有可能有错误，一次只查当前页和当前页的下一页，有待确定
  if(rc == RC::RECORD_NO_MORE_IDX_IN_MEM){
    rc = find_idx_pages();
    if(rc != SUCCESS){
      return rc;
    }
    return get_next_idx_in_memory(rid);
  }
  else{
    if(rc != SUCCESS){
      return rc;
    }
  }
  return SUCCESS;
}

RC BplusTreeScanner::find_idx_pages() {
  RC rc;
  if(!opened_){
    return RC::RECORD_CLOSED;
  }
  if(pinned_page_count_ > 0){
    for(int i = 0; i < pinned_page_count_; i++){
      rc = index_handler_.disk_buffer_pool_->unpin_page(page_handles_ + i);
      if(rc != SUCCESS){
        return rc;
      }
    }
  }
  next_index_of_page_handle_ = 0;
  pinned_page_count_ = 0;

  for(int i = 0; i < num_fixed_pages_; i++){
    if(next_page_num_ <= 0)
      break;
    rc = index_handler_.disk_buffer_pool_->get_this_page(index_handler_.file_name_.c_str(), next_page_num_, page_handles_ + i);
    if(rc != SUCCESS){
      return rc;
    }
    char *pdata;
    rc = index_handler_.disk_buffer_pool_->get_data(page_handles_ + i, &pdata);
    if(rc != SUCCESS){
      return rc;
    }

    IndexNode *node = index_handler_.get_index_node(pdata);
    pinned_page_count_++;
    next_page_num_ = node->rids[index_handler_.file_header_.order-1].page_num;
  }
  if(pinned_page_count_ > 0)
    return SUCCESS;
  return RC::RECORD_EOF;
}

RC BplusTreeScanner::get_next_idx_in_memory(RID *rid) {
  char *pdata;
  IndexNode *node;
  RC rc;
  if(next_index_of_page_handle_ >= pinned_page_count_) {
    return RC::RECORD_NO_MORE_IDX_IN_MEM;
  }

  if(next_page_num_ == -1 && index_in_node_ == -1) {
    return RC::RECORD_EOF;
  }

  for( ; next_index_of_page_handle_ < pinned_page_count_; next_index_of_page_handle_++){
    rc = index_handler_.disk_buffer_pool_->get_data(page_handles_ + next_index_of_page_handle_, &pdata);
    if(rc != SUCCESS){
      return rc;
    }

    node = index_handler_.get_index_node(pdata);
    for( ; index_in_node_ < node->key_num; ){
      if(satisfy_condition(node->keys + index_in_node_ * index_handler_.file_header_.key_length)){
        memcpy(rid,node->rids+index_in_node_,sizeof(RID));
        index_in_node_++;
        return SUCCESS;
      }
      else
        return RC::RECORD_EOF;
    }
    index_in_node_ = 0;
  }
  return RC::RECORD_NO_MORE_IDX_IN_MEM;
}
bool BplusTreeScanner::satisfy_condition(const char *pkey) { // TODO 简化
  int i1=0,i2=0;
  float f1=0,f2=0;
 // int d1=0,d2=0;
  const char *s1=nullptr,*s2=nullptr;

  if(comp_op_ == NO_OP){
    return true;
  }
  bool result = true;
  int offset = RECORD_BITMAP;
  CompOp op = EQUAL_TO;
  long key_map = *(long*)pkey;
  long val_map = *(long*)value_;
  long mask = 0x1;
  for(int i = 0; i < attr_num_; ++i){
    AttrType  attr_type = index_handler_.file_header_.attr_types[i];
    AttrType  type = type_[i];
    const char *ptmp_key = pkey + offset;
    const char *ptmp_value = value_ + offset;
    if(i == attr_num_ - 1)
      op = comp_op_;
    double cmp = 0;
    int attr_length = index_handler_.file_header_.field_len[i];
    switch(attr_type){
      case INTS:
        if(type == INTS) {
          i1=*(int *)ptmp_key;
          i2=*(int *)ptmp_value;
          cmp = i1 - i2;
        } else {
          f1=(float)*(int *)ptmp_key;
          f2=*(float *)ptmp_value;
          cmp = f1 - f2;
        }
        break;
      case FLOATS:
        if(type == FLOATS) {
          f1=*(float *)ptmp_key;
          f2=*(float *)ptmp_value;
          cmp = f1 - f2;
        } else {
          f1=*(int *)ptmp_key;
          f2=(float)*(int *)ptmp_value;
          cmp = f1 - f2;
        }
        break;
      case CHARS:
        s1=ptmp_key;
        s2=ptmp_value;
        cmp = strncmp(s1,s2,attr_length);
        break;
      case DATES:{
        i1=*(int*)ptmp_key;
        i2=*(int*)ptmp_value;
        cmp = (i1 - i2);
        break;
      }
      default:
        LOG_PANIC("Unknown attr type: %d", attr_type);
        return false;
    }
    bool flag = false;
    switch(op){
      case EQUAL_TO:
        if((mask & key_map) == 0){
          flag = ((mask & val_map) == 0) ? true:false;
        }else if((mask & val_map) == 0){
          flag = false;
        }else{
          flag = (cmp == 0);
        }
        break;
      case LESS_THAN:
        if(((mask & key_map)) != (mask & val_map)){
          flag = ((mask & key_map)) == 0 ? true : false;
        }else if(((mask & key_map)) == 0){
          flag = false;
        }else{
          flag = (cmp < 0);
        }
        break;
      case GREAT_THAN:
        if((mask & key_map) != (mask & val_map)){
          flag = (mask & key_map) == 0 ? false : true;
        }else if((mask & key_map) == 0){
          flag = false;
        }else{
          flag = (cmp > 0);
        }
        break;
      case LESS_EQUAL:
        if((mask & key_map) != (mask & val_map)){
          flag = (mask & key_map) == 0 ? true : false;
        }else if((mask & key_map) == 0){
          flag = true;
        }else{
          flag = cmp <= 0;
        }
        break;
      case GREAT_EQUAL:
        if((mask & key_map) != (mask & val_map)){
          flag = (mask & key_map) == 0 ? false : true;
        }else if((mask & key_map) == 0){
          flag = true;
        }else{
          flag = cmp >= 0;
        }
        break;
      case NOT_EQUAL:
        if((mask & key_map) != (mask & val_map)){
          flag = true;
        }else if((mask & key_map) == 0){
          flag = false;
        }else{
          flag = cmp != 0;
        }
        break;
      default:
        LOG_PANIC("Unknown comp op: %d", comp_op_);
        return false;
    }
    result = result & flag;
    if(!result){
      return result;
    }
    offset += index_handler_.file_header_.field_len[i];
  }
  return result;
}
