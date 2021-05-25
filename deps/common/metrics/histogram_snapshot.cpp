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
// Created by Longda on 2021/4/20.
//

#include "histogram_snapshot.h"
#include <algorithm>
#include <math.h>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <stdio.h>

namespace common {


HistogramSnapShot::HistogramSnapShot()
{
}

HistogramSnapShot::HistogramSnapShot(const std::vector<double>& collection)
{
  setCollection(collection);
}

HistogramSnapShot::~HistogramSnapShot()
{
}

void HistogramSnapShot::setCollection(const std::vector<double>& collection)
{
    if (collection.empty())
    {
        return;
    }
    
    data = collection;
    std::sort(data.begin(), data.end());
}

size_t HistogramSnapShot::size() const
{
    return data.size();
}

double HistogramSnapShot::getValue(double quantile)
{
    if (quantile > 1.0f)
    {
        quantile = 1.0f;
    }
    
    if (quantile < 0.0f)
    {
        quantile = 0.0f;
    }
    
    if (data.empty())
    {
        return 0.0f;
    }
    
    double pos = quantile * (data.size() + 1);
    
    if (pos < 1)
    {
        return data[0];
    }
    
    if (pos >= data.size())
    {
        return data[data.size() - 1];
    }
    
    double lower = data[(int) pos - 1];
    double upper = data[(int) pos];
    
    return lower + (pos - floor(pos)) * (upper - lower);
}

double HistogramSnapShot::getMedian()
{
    return getValue(0.5f);
}

double HistogramSnapShot::get75th()
{
    return getValue(0.75f);
}

double HistogramSnapShot::get90th()
{
    return getValue(0.90f);
}

double HistogramSnapShot::get95th()
{
    return getValue(0.95f);
}
double HistogramSnapShot::get99th()
{
    return getValue(0.99f);
    
}
double HistogramSnapShot::get999th()
{
    return getValue(0.999f);
}

double HistogramSnapShot::getMax()
{
    if (data.empty())
    {
        return 0.0f;
    }
    
    return static_cast<double>(*data.rbegin());
}

double HistogramSnapShot::getMin()
{
    if (data.empty())
    {
        return 0.0f;
    }
    
    return static_cast<double>(*data.begin());
}

double HistogramSnapShot::getMean()
{
    if (data.empty())
    {
        return 0.0f;
    }
    
    return std::accumulate(data.begin(), data.end(), (double)0) * 1.0f / data.size();
}


const std::vector<double> & HistogramSnapShot::getValues()
{
    return data;
}

std::string HistogramSnapShot::toString() {
  std::stringstream oss;
  oss << "mean:" << getMean() << ",min:" << getMin() << ",max:" << getMax()
      << ",median:" << getMedian() << ", 75th:" << get75th()
      << ",90th:" << get90th() << ",99th:" << get99th()
      << ",999th:" << get999th();

  return oss.str();
}


} // namespace common