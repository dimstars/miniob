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
#include "pthread.h"
#include "stdio.h"
#include "string.h"
#include <iomanip>
#include <sstream>
#include <string>

#include "common/time/datetime.h"
namespace common {

DateTime::DateTime(std::string &xmlStr) {
  tm tmp;
  sscanf(xmlStr.c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ", &tmp.tm_year,
         &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
  m_date = julianDate(tmp.tm_year, tmp.tm_mon, tmp.tm_mday);
  m_time = makeHMS(tmp.tm_hour, tmp.tm_min, tmp.tm_sec, 0);
}

time_t DateTime::StrtoTimeT(std::string &xmlStr) {
  tm tmp;
  sscanf(xmlStr.c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ", &tmp.tm_year,
         &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
  m_date = julianDate(tmp.tm_year, tmp.tm_mon, tmp.tm_mday);
  m_time = makeHMS(tmp.tm_hour, tmp.tm_min, tmp.tm_sec, 0);
  return toTimeT();
}

std::string DateTime::TimeTtoStr(int timet) {
  std::ostringstream oss;
  oss << std::dec << std::setw(10) << timet;
  return oss.str();
}

std::string DateTime::TimeTtoXmlStr(time_t timet) {
  std::string retVal;
  std::ostringstream oss;
  struct tm tmbuf;
  tm *tmInfo = gmtime_r(&timet, &tmbuf);
  oss << tmInfo->tm_year + 1900 << "-";
  if ((tmInfo->tm_mon + 1) <= 9)
    oss << "0";
  oss << tmInfo->tm_mon + 1 << "-";
  if (tmInfo->tm_mday <= 9)
    oss << "0";
  oss << tmInfo->tm_mday << "T";
  if (tmInfo->tm_hour <= 9)
    oss << "0";
  oss << tmInfo->tm_hour << ":";
  if (tmInfo->tm_min <= 9)
    oss << "0";
  oss << tmInfo->tm_min << ":";
  if (tmInfo->tm_sec <= 9)
    oss << "0";
  oss << tmInfo->tm_sec << "Z";
  retVal = oss.str();
  return retVal;
}

std::string DateTime::StrtoTimeTStr(std::string &xmlStr) {
  tm tmp;
  std::ostringstream oss;
  sscanf(xmlStr.c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ", &tmp.tm_year,
         &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
  m_date = julianDate(tmp.tm_year, tmp.tm_mon, tmp.tm_mday);
  m_time = makeHMS(tmp.tm_hour, tmp.tm_min, tmp.tm_sec, 0);
  time_t timestamp = toTimeT();
  oss << std::dec << std::setw(10) << timestamp;
  return oss.str();
}

time_t DateTime::nowtimet() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec;;
}

DateTime DateTime::now() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return fromTimeT(tv.tv_sec, tv.tv_usec / 1000);
}

//! Return date and time as a string in Xml Schema date-time format
std::string DateTime::toXmlDateTime() {

  std::string retVal;
  tm tmInfo;
  std::ostringstream oss;

  tmInfo = toTm();
  oss << tmInfo.tm_year + 1900 << "-";
  if ((tmInfo.tm_mon + 1) <= 9)
    oss << "0";
  oss << tmInfo.tm_mon + 1 << "-";
  if (tmInfo.tm_mday <= 9)
    oss << "0";
  oss << tmInfo.tm_mday << "T";
  if (tmInfo.tm_hour <= 9)
    oss << "0";
  oss << tmInfo.tm_hour << ":";
  if (tmInfo.tm_min <= 9)
    oss << "0";
  oss << tmInfo.tm_min << ":";
  if (tmInfo.tm_sec <= 9)
    oss << "0";
  oss << tmInfo.tm_sec << "Z";
  retVal = oss.str();
  return retVal;
}

time_t DateTime::addDuration(std::string xmlDuration) {
  addDurationDateTime(xmlDuration);
  return toTimeT();
}

void DateTime::addDurationDateTime(std::string xmlDuration) {
  // start datetime values
  int sYear, sMonth, sDay;
  int sHour, sMin, sSec, sMillis = 0;
  getYMD(sYear, sMonth, sDay);
  getHMS(sHour, sMin, sSec, sMillis);

  // temp values
  int tmpMonth, tmpSec, tmpMin, tmpHour, tmpDay;

  // duration values
  struct tm durT;
  parseDuration(xmlDuration, durT);

  // end values
  int eYear, eMonth, eDay, eHour, eMin, eSec, eMillis = 0;

  // months
  tmpMonth = sMonth + durT.tm_mon;
  eMonth = ((tmpMonth - 1) % 12) + 1;
  int carryMonth = ((tmpMonth - 1) / 12);

  // years
  eYear = sYear + durT.tm_year + carryMonth;

  // seconds
  tmpSec = sSec + durT.tm_sec;
  eSec = tmpSec % 60;
  int carrySec = tmpSec / 60;

  // minutes
  tmpMin = sMin + durT.tm_min + carrySec;
  eMin = tmpMin % 60;
  int carryMin = tmpMin / 60;

  // hours
  tmpHour = sHour + durT.tm_hour + carryMin;
  eHour = tmpHour % 24;
  int carryHr = tmpHour / 24;

  // days
  if (sDay > maxDayInMonthFor(eYear, eMonth)) {
    tmpDay = maxDayInMonthFor(eYear, eMonth);
  } else {
    if (sDay < 1) {
      tmpDay = 1;
    } else {
      tmpDay = sDay;
    }
  }
  eDay = tmpDay + durT.tm_mday + carryHr;
  int carryDay = 0;
  while (true) {
    if (eDay < 1) {
      eDay = eDay + maxDayInMonthFor(eYear, eMonth - 1);
      carryDay = -1;
    } else {
      if (eDay > maxDayInMonthFor(eYear, eMonth)) {
        eDay -= maxDayInMonthFor(eYear, eMonth);
        carryDay = 1;
      } else {
        break;
      }
    }
    tmpMonth = eMonth + carryDay;
    eMonth = ((tmpMonth - 1) % 12) + 1;
    eYear = eYear + (tmpMonth - 1) / 12;
  }
  m_date = julianDate(eYear, eMonth, eDay);
  m_time = makeHMS(eHour, eMin, eSec, eMillis);
  return;
}

int DateTime::maxDayInMonthFor(int yr, int month) {
  int tmpMonth = ((month - 1) % 12) + 1;
  int tmpYear = yr + ((tmpMonth - 1) / 12);

  if (tmpMonth == MON_JAN || tmpMonth == MON_MAR || tmpMonth == MON_MAY ||
    tmpMonth == MON_JUL || tmpMonth == MON_AUG || tmpMonth == MON_OCT ||
    tmpMonth == MON_DEC) {
    return 31;
  } else {
    if (tmpMonth == MON_APR || tmpMonth == MON_JUN || tmpMonth == MON_SEP ||
      tmpMonth == MON_NOV)
      return 30;
    else {
      if (tmpMonth == MON_FEB && ((0 == tmpYear % 400) ||
        ((0 != tmpYear % 100) && 0 == tmpYear % 4))) {
        return 29;
      } else
        return 28;
    }
  }
}

void DateTime::parseDuration(std::string durStr, struct tm &tmT) {
  std::string::size_type index = 0;
  bzero(&tmT, sizeof(tmT));
  if (durStr[index] != 'P') {
    return;
  }
  int indT = durStr.find('T', 0);

  index++;
  int indY = durStr.find('Y', index);
  if (indY != -1) {
    int sign = 1;
    if (durStr[index] == '-') {
      sign = -1;
      index++;
    }
    std::string sY = durStr.substr(index, indY);
    sscanf(sY.c_str(), "%d", &tmT.tm_year);
    tmT.tm_year *= sign;
    index = indY + 1;
  }

  int indM = durStr.find('M', index);
  if ((indM != -1) && (((indT > -1) && (indM < indT)) || indT == -1)) {
    int sign = 1;
    if (durStr[index] == '-') {
      sign = -1;
      index++;
    }
    sscanf(durStr.substr(index, indM).c_str(), "%d", &tmT.tm_mon);
    tmT.tm_mon *= sign;
    index = indM + 1;
  }

  int indD = durStr.find('D', index);
  int sign = 1;
  if (indD != -1) {
    if (durStr[index] == '-') {
      sign = -1;
      index++;
    }
    sscanf(durStr.substr(index, indD).c_str(), "%d", &tmT.tm_mday);
    tmT.tm_mday *= sign;
    index = indD + 1;
  }

  if (indT == -1) {
    tmT.tm_hour = tmT.tm_min = tmT.tm_sec = 0;
    return;
  }
  index = indT + 1;

  int indH = durStr.find('H', index);
  if (indH != -1) {
    int sign = 1;
    if (durStr[index] == '-') {
      sign = -1;
      index++;
    }
    sscanf(durStr.substr(index, indH).c_str(), "%d", &tmT.tm_hour);
    tmT.tm_hour *= sign;
    index = indH + 1;
  }

  int indMin = durStr.find('M', index);
  if (indMin != -1) {
    int sign = 1;
    if (durStr[index] == '-') {
      sign = -1;
      index++;
    }
    sscanf(durStr.substr(index, indMin).c_str(), "%d", &tmT.tm_min);
    tmT.tm_min *= sign;
    index = indMin + 1;
  }

  int indS = durStr.find('S', index);
  if (indS != -1) {
    int sign = 1;
    if (durStr[index] == '-') {
      sign = -1;
      index++;
    }
    sscanf(durStr.substr(index, indS).c_str(), "%d", &tmT.tm_sec);
    tmT.tm_sec *= sign;
  }
  return;
}
// generate OBJ_ID_TIMESTMP_DIGITS types unique timestamp string
// caller doesn't need get any lock
#define OBJ_ID_TIMESTMP_DIGITS 14
std::string Now::unique() {
  struct timeval tv;
  u64_t temp;
  static u64_t lastUnique = 0;
#if defined(LINUX)
  static pthread_mutex_t mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
#elif defined(__MACH__)
  static pthread_mutex_t mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;
#endif
  gettimeofday(&tv, NULL);
  temp = (((u64_t) tv.tv_sec) << 20) + tv.tv_usec;
  pthread_mutex_lock(&mutex);
  if (temp > lastUnique) {
    // record last timeStamp
    lastUnique = temp;
  } else {
    // increase the last timeStamp and use it as unique timestamp
    // as NTP may sync local time clock backward.
    // after time catch up, will change back to use time again.
    lastUnique++;
    // set the new lastUnique as unique timestamp
    temp = lastUnique;
  }
  pthread_mutex_unlock(&mutex);

  // TODO further refine below code, the common time unique function
  //      should not cover OBJ_ID_TIMESTMP_DIGITS, which is only
  //      related with the object id.
  std::ostringstream oss;
  oss << std::hex << std::setw(OBJ_ID_TIMESTMP_DIGITS) << std::setfill('0')
      << temp;
  return oss.str();
}

bool DateTime::isValidXMLDatetime(const std::string &str) {
  // check length. 20 is the length of a xml date
  if (str.length() != 20)
    return false;

  // check each character is correct
  const char *const flag = "0000-00-00T00:00:00Z";
  for (unsigned int i = 0; i < str.length(); ++i) {
    if (flag[i] == '0') {
      if (!isdigit(str[i]))
        return false;
    } else if (flag[i] != str[i]) {
      return false;
    }
  }

  // check month, date, hour, min, second is valid
  tm tmp;
  int ret =
    sscanf(str.c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ", &tmp.tm_year,
           &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);

  if (ret != 6)
    return false; // should have 6 match

  if (tmp.tm_mon > 12 || tmp.tm_mon <= 0)
    return false;
  if (tmp.tm_mday > 31 || tmp.tm_mday <= 0)
    return false;
  if (tmp.tm_hour > 24 || tmp.tm_hour < 0)
    return false;
  if (tmp.tm_min > 60 || tmp.tm_min < 0)
    return false;
  if (tmp.tm_sec > 60 || tmp.tm_sec < 0)
    return false;

  return true;
}

} //namespace common