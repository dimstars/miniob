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
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/execution_node.h"
#include "storage/common/table.h"
#include "common/log/log.h"
#include "sql/executor/serial_generator.h"

SelectExeNode::SelectExeNode() : table_(nullptr) {
}

RC
SelectExeNode::init(Trx *trx, Table *table, TupleSchema &&tuple_schema, std::vector<DefaultConditionFilter> &&condition_filters) {
  trx_ = trx;
  table_ = table;
  tuple_schema_ = tuple_schema;
  condition_filters_ = condition_filters;
  return RC::SUCCESS;
}

void record_reader(const char *data, void *context) {
  TupleRecordConverter *converter = (TupleRecordConverter *)context;
  converter->add_record(data);
}
RC SelectExeNode::execute(TupleSet &tuple_set) {
  CompositeConditionFilter condition_filter;
  condition_filter.init(condition_filters_.data(), condition_filters_.size());

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  TupleRecordConverter converter(table_, tuple_set);
  return table_->scan_record(trx_, &condition_filter, -1, (void *)&converter, record_reader);
}

////////////////////////////////////////////////////////////////////////////////
class TupleConditionFilter {
public:
  TupleConditionFilter()  = default;

  RC init(const std::vector<TupleSet> &tuple_set_list, const Condition *condition) {
    if (condition->op < EQual || condition->op > GreatT) {
      return RC::INVALID_ARGUMENT;
    }

    int tuple_set_size = tuple_set_list.size();
    for (int i = 0; i < tuple_set_size; i++) {
      const TupleSchema &schema = tuple_set_list[i].schema();
      if (index_in_tuple_set_left_ == -1) {
        index_in_tuple_left_ = schema.index_of_field(condition->lhsAttr.relName, condition->lhsAttr.attrName);
        if (index_in_tuple_left_ >= 0) {
          index_in_tuple_set_left_ = i;
        }
      }
      if (index_in_tuple_set_right_ == -1) {
        index_in_tuple_right_ = schema.index_of_field(condition->rhsAttr.relName, condition->rhsAttr.attrName);
        if (index_in_tuple_right_ >= 0) {
          index_in_tuple_set_right_ = i;
        }
      }
    }

    if (index_in_tuple_set_left_ == -1 || index_in_tuple_set_right_ == -1) {
      LOG_ERROR("Failed to address condition. left attr=%s.%s, right attr=%s.%s",
                condition->lhsAttr.relName, condition->lhsAttr.attrName,
                condition->rhsAttr.relName, condition->rhsAttr.attrName);
      return RC::GENERIC_ERROR;
    }

    const TupleField &left_field = tuple_set_list[index_in_tuple_set_left_].schema().field(index_in_tuple_left_);
    const TupleField &right_field = tuple_set_list[index_in_tuple_set_right_].schema().field(index_in_tuple_right_);
    if (left_field.type() != right_field.type()) {
      LOG_ERROR("Condition on both side have difference type: left %s, right %s",
                left_field.to_string().c_str(), right_field.to_string().c_str());
      return RC::GENERIC_ERROR;
    }

    attr_type_ = left_field.type();
    comp_op_ = condition->op;
    return RC::SUCCESS;
  }

  bool filter(const std::vector<const Tuple *> &tuples) const {
    const TupleValue &left_value = tuples[index_in_tuple_set_left_]->get(index_in_tuple_left_);
    const TupleValue &right_value = tuples[index_in_tuple_set_right_]->get(index_in_tuple_right_);

    int result = left_value.compare(right_value);

    switch (comp_op_) {
      case EQual: return result == 0;
      case LEqual: return result <= 0;
      case NEqual: return result != 0;
      case LessT: return result < 0;
      case GEqual: return result >= 0;
      case GreatT: return result > 0;
      default: { // never happen
        LOG_PANIC("Found invalid compare op: %d", comp_op_);
        return false;
      }
    }
  }
private:
  CompOp comp_op_;
  AttrType attr_type_;
  int   index_in_tuple_set_left_ = -1;
  int   index_in_tuple_left_ = -1;
  int   index_in_tuple_set_right_ = -1;
  int   index_in_tuple_right_ = -1;
};
class CompositeTupleConditionFilter {
public:
  RC init(const std::vector<TupleSet> &tuple_set_list, const std::vector<const Condition *> &conditions) {
    RC rc = RC::SUCCESS;
    int size = conditions.size();
    condition_filters_.resize(size);
    for (int i = 0; i < size; i++) {
      rc = condition_filters_[i].init(tuple_set_list, conditions[i]);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    return rc;
  }

  bool filter(const std::vector<const Tuple *> &tuples) const {
    for (const auto &condition_filter: condition_filters_) {
      if (!condition_filter.filter(tuples)) {
        return false;
      }
    }
    return true;
  }

private:
  std::vector<TupleConditionFilter> condition_filters_;
};


class TupleExtractor {
public:
  TupleExtractor() = default;

  RC init(const TupleSchema &dest_schema, const std::vector<TupleSet> &source_tuple_sets) {
    field_index_of_tuple_set_.reserve(dest_schema.fields().size());
    field_index_of_tuple_.reserve(dest_schema.fields().size());

    const int tuple_sets_size = source_tuple_sets.size();
    for (const TupleField &field : dest_schema.fields()) {
      int index_of_tuple_set = -1;
      int index_of_tuple = -1;

      for (int i = 0; i < tuple_sets_size; i++) {
        const TupleSet & tuple_set = source_tuple_sets[i];
        const int source_tuple_field_size = tuple_set.schema().fields().size();
        for (int j = 0; j < source_tuple_field_size; j++) {
          if (0 == strcmp(tuple_set.schema().field(j).table_name(), field.table_name()) &&
              0 == strcmp(tuple_set.schema().field(j).field_name(), field.field_name())) {
            index_of_tuple_set = i;
            index_of_tuple = j;
            break;
          }
        }
        if (index_of_tuple_set != -1) {
          break;
        }
      }

      if (index_of_tuple_set == -1) {
        LOG_PANIC("Cannot find field (%s) in source tuple sets", field.to_string().c_str());
        return RC::GENERIC_ERROR;
      }
      field_index_of_tuple_set_.push_back(index_of_tuple_set);
      field_index_of_tuple_.push_back(index_of_tuple);
    }
    return RC::SUCCESS;
  }

  void extract(const std::vector<const Tuple *> tuples, Tuple &tuple_out) {
    const int size = field_index_of_tuple_set_.size();
    for (int i = 0; i < size; i++) {
      tuple_out.add(tuples[field_index_of_tuple_set_[i]]->get_pointer(field_index_of_tuple_[i]));
    }
  }

private:
  std::vector<int> field_index_of_tuple_set_;
  std::vector<int> field_index_of_tuple_;
};

JoinExeNode::~JoinExeNode() {
  delete condition_filter_;
  condition_filter_ = nullptr;
}

RC JoinExeNode::init(const std::vector<TupleSet> &tuple_set_list, TupleSchema &&tuple_schema,
                     std::vector<const Condition *> &&condition_filters) {
  tuple_set_list_ = &tuple_set_list;
  tuple_schema_ = std::move(tuple_schema);
  CompositeTupleConditionFilter *condition_filter = new CompositeTupleConditionFilter;
  RC rc = condition_filter->init(tuple_set_list, condition_filters);
  if (rc != RC::SUCCESS) {
    delete condition_filter;
  } else {
    condition_filter_ = condition_filter;
  }
  return rc;
}

RC JoinExeNode::execute(TupleSet &tuple_set) {
  tuple_set.set_schema(tuple_schema_);

  const int tuple_set_list_size = tuple_set_list_->size();
  std::vector<const std::vector<Tuple> *> tuple_set_list_copy;
  tuple_set_list_copy.reserve(tuple_set_list_size);
  for (const TupleSet & tuple_set_item: *tuple_set_list_) {
    tuple_set_list_copy.push_back(&tuple_set_item.tuples());
  }

  std::vector<const Tuple *> tuples(tuple_set_list_size);

  SerialGenerator serial_generator;
  using TupleMetaSerialIterator = MetaSerialIterator<std::vector<Tuple>::const_iterator, SerialAddressFiller<const Tuple>>;
  for (int i = 0; i < tuple_set_list_size; i++) {
    const std::vector<Tuple> * source_tuples = tuple_set_list_copy[i];
     TupleMetaSerialIterator  *meta_serial_iterator =
            new TupleMetaSerialIterator (
                    source_tuples->begin(), source_tuples->end(), SerialAddressFiller<const Tuple>(&tuples[i]));
    serial_generator.append(meta_serial_iterator);
  }

  TupleExtractor tuple_extractor;
  RC rc = tuple_extractor.init(tuple_schema_, *tuple_set_list_);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  Tuple tuple_extracted;
  for ( ; serial_generator.has_more(); serial_generator.forward()) {
    serial_generator.fill();
    if (condition_filter_->filter(tuples)) {
      // collect one
      tuple_extractor.extract(tuples, tuple_extracted);
      tuple_set.add(std::move(tuple_extracted));
    }
  }
  return RC::SUCCESS;
}
