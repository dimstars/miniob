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
// Created by wangyunlai.wyl on 2021/5/21.
//

#ifndef __OBSERVER_SQL_EXECUTOR_SERIAL_GENERATOR_H_
#define __OBSERVER_SQL_EXECUTOR_SERIAL_GENERATOR_H_

class SerialIterator {
public:
  virtual ~SerialIterator() = default;

  virtual bool has_more() const = 0;
  virtual void forward() = 0;
  virtual void fill() = 0;
  virtual void rewind() = 0;
};

template <class Type>
class SerialValueFiller {
public:
  SerialValueFiller(Type *pointer): pointer_(pointer) {

  }
  void operator() (const Type &value) const {
    *pointer_ = value;
  }

private:
  Type *pointer_;
};

template <class Type>
class SerialAddressFiller {
public:
  SerialAddressFiller(Type **pointer): pointer_(pointer) {

  }
  void operator() (const Type &value) const {
    *pointer_ = &value;
  }

private:
  Type **pointer_;
};

/**
 * example:
 * Iterator = std::vector<Tuple>::iterator
 */
template <class Iterator, class Filler>
class MetaSerialIterator : public SerialIterator {
public:
  using iterator_t = Iterator;
public:
  MetaSerialIterator(iterator_t begin, iterator_t end, Filler filler) :
          begin_(begin), end_(end), iter_(begin), filler_(filler) {
  }

  ~MetaSerialIterator() override = default;

  bool has_more() const override {
    return iter_ != end_;
  }

  void forward() override {
    ++iter_;
  }

  void fill() override {
    filler_(*iter_);
  }

  void rewind() override {
    iter_ = begin_;
  }

private:
  const iterator_t begin_;
  const iterator_t end_;
  iterator_t iter_;
  Filler filler_;
};

class DoubleSerialIterator : public SerialIterator {
public:
  DoubleSerialIterator(SerialIterator *iter1, SerialIterator *iter2) : iter1_(iter1), iter2_(iter2) {
  }

  ~DoubleSerialIterator() override {
    delete iter1_;
    iter1_ = nullptr;
    delete iter2_;
    iter2_ = nullptr;
  }

  bool has_more() const override {
    return iter1_->has_more() && iter2_->has_more();
  }

  void forward() override {
    iter1_->forward();
    if (!iter1_->has_more()) {
      iter1_->rewind();
      iter2_->forward();
    }
  }

  void fill() override {
    iter1_->fill();
    iter2_->fill();
  }

  void rewind() override {
    iter1_->rewind();
    iter2_->rewind();
  }

private:
  SerialIterator *iter1_;
  SerialIterator *iter2_;
};

/**
 * 示例
 * Type = Tuple *
 * SourceIterator = vector<vector<Tuple> *>::iterator
 * ResultIterator = vector<Tuple *>::iterator
 * @tparam Type
 * @tparam SourceIterator
 * @tparam ResultIterator
 */
class SerialGenerator : public SerialIterator{
public: // TODO 优化这里复杂的代码
  SerialGenerator() {
    iterator_ = nullptr;
  }

  void append(SerialIterator *serial_iterator) {
    if (iterator_ == nullptr) {
      iterator_ = serial_iterator;
    } else {
      iterator_ = new DoubleSerialIterator(iterator_, serial_iterator);
    }
  }

  ~SerialGenerator() {
    delete iterator_;
    iterator_ = nullptr;
  }

  bool has_more() const override {
    return iterator_->has_more();
  }

  void forward() override {
    iterator_->forward();
  }

  void fill() override {
    iterator_->fill();
  }

  void rewind() override {
    iterator_->rewind();
  }

private:
  SerialIterator *iterator_ = nullptr;
};
#endif //__OBSERVER_SQL_EXECUTOR_SERIAL_GENERATOR_H_
