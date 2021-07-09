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
// Created by wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_NUM ("field_num");
const static Json::StaticString EXTENDED_ATTR ("extended_attr");


RC IndexMeta::init(const char *name, std::vector<const FieldMeta*> &fields, bool unique, int field_num) {
  if (nullptr == name || common::is_blank(name)) {
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  for(int i = 0; i < fields.size(); ++i){
    fields_[i] = fields[i]->name();
  }
  field_num_ = field_num;
  extended_attr_ = unique ? std::string("unique"):"";
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const {
  json_value[FIELD_NAME] = name_;
  int len = 0;
  for(int i = 0; i < fields_.size(); ++i){
    len += strlen(fields_[i]);
  }
  char* tmp = (char*)malloc(sizeof(char) * (len + fields_.size()));
  for(int i = 0; i < fields_.size(); ++i){
    strcat(tmp,fields_[i]);
    strcat(tmp," ");
  }
  tmp[len + fields_.size() - 1] = '\0';

  json_value[FIELD_FIELD_NAME] = tmp;
  json_value[EXTENDED_ATTR] = extended_attr_;
  json_value[FIELD_NUM] = field_num_;

  free(tmp);
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index) {
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  const Json::Value &extended_attr_ = json_value[EXTENDED_ATTR];
  const Json::Value &field_num = json_value[FIELD_NUM];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  if (!field_value.isString()) {
    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
              name_value.asCString(), field_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  if(!field_num.isInt()){
    LOG_ERROR("Field name of index [%s] is not Int. json value=%s",
              field_num.asCString(), field_num.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  int num = field_num.asInt();

  const char *str = field_value.asCString();
  const FieldMeta *field;
  std::vector<const FieldMeta*> fields;
  for(int i = 0; i < num; ++i){
      char *p = strchr(str,' ');
      field = table.field(str + );
      if (nullptr == field) {
        LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
        return RC::SCHEMA_FIELD_MISSING;
      }
      fields.push_back(field);
  }
  return index.init(name_value.asCString(), fields, extended_attr_.asCString() == "unique", field_num.asInt());
}

const char *IndexMeta::name() const {
  return name_.c_str();
}
const char *IndexMeta::field(int pos) const{
  return fields_[i];
}
const std::vector<const char*> &IndexMeta::field() const {
  return fields_;
}

const char *IndexMeta::extended_attr() const {
  return extended_attr_.c_str();
}

int IndexMeta::field_num() const{
  return field_num_;
}

void IndexMeta::desc(std::ostream &os) const {
  os << "index name=" << name_
     << ", field=";
  for(int i = 0; i < field_num_; ++i){
    os<< fields_[i] << " ";
  }
  os << ", extended attr="<< extended_attr_
     << ", field num="<<field_num_;
}