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

#include <dirent.h>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/math/regex.h"
#include "common/mm/mem.h"

namespace common {


int readFromFile(const std::string &fileName, char *&outputData,
                 size_t &fileSize) {
  FILE *file = fopen(fileName.c_str(), "rb");
  if (file == NULL) {
    std::cerr << "Failed to open file " << fileName << SYS_OUTPUT_FILE_POS
              << SYS_OUTPUT_ERROR << std::endl;
    return -1;
  }

  // fseek( file, 0, SEEK_END );
  // size_t fsSize = ftell( file );
  // fseek( file, 0, SEEK_SET );

  char buffer[4 * ONE_KILO];
  size_t readSize = 0;
  size_t oneRead = 0;

  char *data = NULL;
  do {
    memset(buffer, 0, sizeof(buffer));
    oneRead = fread(buffer, 1, sizeof(buffer), file);
    if (ferror(file)) {
      std::cerr << "Failed to read data" << fileName << SYS_OUTPUT_FILE_POS
                << SYS_OUTPUT_ERROR << std::endl;
      fclose(file);
      return -1;
    }

    data = (char *)lrealloc(data, readSize + oneRead);
    if (data == NULL) {
      std::cerr << "Failed to alloc memory for " << fileName
                << SYS_OUTPUT_FILE_POS << SYS_OUTPUT_ERROR << std::endl;
      lfree(data);
      fclose(file);
      return -1;
    } else {
      memcpy(data + readSize, buffer, oneRead);
      readSize += oneRead;
    }

  } while (feof(file) == 0);

  fclose(file);

  outputData = data;
  fileSize = readSize;
  return 0;
}

int writeToFile(const std::string &fileName, const char *data, u32_t dataSize,
                const char *openMode) {
  FILE *file = fopen(fileName.c_str(), openMode);
  if (file == NULL) {
    std::cerr << "Failed to open file " << fileName << SYS_OUTPUT_FILE_POS
              << SYS_OUTPUT_ERROR << std::endl;
    return -1;
  }

  u32_t leftSize = dataSize;
  const char *buffer = data;
  while (leftSize > 0) {
    int writeCount = fwrite(buffer, 1, leftSize, file);
    if (writeCount <= 0) {
      std::cerr << "Failed to open file " << fileName << SYS_OUTPUT_FILE_POS
                << SYS_OUTPUT_ERROR << std::endl;
      fclose(file);
      return -1;
    } else {
      leftSize -= writeCount;
      buffer += writeCount;
    }
  }

  fclose(file);

  return 0;
}

int getFileLines(const std::string &fileName, u64_t &lineNum) {
  lineNum = 0;

  char line[4 * ONE_KILO] = {0};

  std::ifstream ifs(fileName.c_str());
  if (!ifs) {
    return -1;
  }

  while (ifs.good()) {
    line[0] = 0;
    ifs.getline(line, sizeof(line));
    char *lineStrip = strip(line);
    if (strlen(lineStrip)) {
      lineNum++;
    }
  }

  ifs.close();
  return 0;
}

int getFileNum(u64_t &fileNum, const std::string &path,
               const std::string &pattern, bool recursive) {
  try {
    DIR *dirp = NULL;
    dirp = opendir(path.c_str());
    if (dirp == NULL) {
      std::cerr << "Failed to opendir " << path << SYS_OUTPUT_FILE_POS
                << SYS_OUTPUT_ERROR << std::endl;
      return -1;
    }

    std::string fullPath;
    struct dirent *entry = NULL;
    struct stat fs;
    while ((entry = readdir(dirp)) != NULL) {
      // don't care ".", "..", ".****" hidden files
      if (!strncmp(entry->d_name, ".", 1)) {
        continue;
      }

      fullPath = path;
      if (path[path.size() - 1] != FILE_PATH_SPLIT) {
        fullPath += FILE_PATH_SPLIT;
      }
      fullPath += entry->d_name;
      memset(&fs, 0, sizeof(fs));
      if (stat(fullPath.c_str(), &fs) < 0) {
        std::cout << "Failed to stat " << fullPath << SYS_OUTPUT_FILE_POS
                  << SYS_OUTPUT_ERROR << std::endl;
        continue;
      }

      if (fs.st_mode & S_IFDIR) {
        if (recursive == 0) {
          continue;
        }

        if (getFileNum(fileNum, fullPath, pattern, recursive) < 0) {
          closedir(dirp);
          return -1;
        }
      }

      if (!(fs.st_mode & S_IFREG)) {
        // not regular files
        continue;
      }

      if (pattern.empty() == false &&
          regex_match(entry->d_name, pattern.c_str())) {
        // Don't match
        continue;
      }

      fileNum++;
    }

    closedir(dirp);

    return 0;
  } catch (...) {
    std::cerr << "Failed to get file num " << path << SYS_OUTPUT_FILE_POS
              << SYS_OUTPUT_ERROR << std::endl;
  }
  return -1;
}

int getFileList(std::vector<std::string> &fileList, const std::string &path,
                const std::string &pattern, bool recursive) {
  try {
    DIR *dirp = NULL;
    dirp = opendir(path.c_str());
    if (dirp == NULL) {
      std::cerr << "Failed to opendir " << path << SYS_OUTPUT_FILE_POS
                << SYS_OUTPUT_ERROR << std::endl;
      return -1;
    }

    std::string fullPath;
    struct dirent *entry = NULL;
    struct stat fs;
    while ((entry = readdir(dirp)) != NULL) {
      // don't care ".", "..", ".****" hidden files
      if (!strncmp(entry->d_name, ".", 1)) {
        continue;
      }

      fullPath = path;
      if (path[path.size() - 1] != FILE_PATH_SPLIT) {
        fullPath += FILE_PATH_SPLIT;
      }
      fullPath += entry->d_name;
      memset(&fs, 0, sizeof(fs));
      if (stat(fullPath.c_str(), &fs) < 0) {
        std::cout << "Failed to stat " << fullPath << SYS_OUTPUT_FILE_POS
                  << SYS_OUTPUT_ERROR << std::endl;
        continue;
      }

      if (fs.st_mode & S_IFDIR) {
        if (recursive == 0) {
          continue;
        }

        if (getFileList(fileList, fullPath, pattern, recursive) < 0) {
          closedir(dirp);
          return -1;
        }
      }

      if (!(fs.st_mode & S_IFREG)) {
        // regular files
        continue;
      }

      if (pattern.empty() == false &&
          regex_match(entry->d_name, pattern.c_str())) {
        // Don't match
        continue;
      }

      fileList.push_back(fullPath);
    }

    closedir(dirp);
    return 0;
  } catch (...) {
    std::cerr << "Failed to get file list " << path << SYS_OUTPUT_FILE_POS
              << SYS_OUTPUT_ERROR << std::endl;
  }
  return -1;
}

int getDirList(std::vector<std::string> &dirList, const std::string &path,
               const std::string &pattern) {
  try {
    DIR *dirp = NULL;
    dirp = opendir(path.c_str());
    if (dirp == NULL) {
      std::cerr << "Failed to opendir " << path << SYS_OUTPUT_FILE_POS
                << SYS_OUTPUT_ERROR << std::endl;
      return -1;
    }

    std::string fullPath;
    struct dirent *entry = NULL;
    struct stat fs;
    while ((entry = readdir(dirp)) != NULL) {
      // don't care ".", "..", ".****" hidden files
      if (!strncmp(entry->d_name, ".", 1)) {
        continue;
      }

      fullPath = path;
      if (path[path.size() - 1] != FILE_PATH_SPLIT) {
        fullPath += FILE_PATH_SPLIT;
      }
      fullPath += entry->d_name;
      memset(&fs, 0, sizeof(fs));
      if (stat(fullPath.c_str(), &fs) < 0) {
        std::cout << "Failed to stat " << fullPath << SYS_OUTPUT_FILE_POS
                  << SYS_OUTPUT_ERROR << std::endl;
        continue;
      }

      if ((fs.st_mode & S_IFDIR) == 0) {
        continue;
      }

      if (pattern.empty() == false &&
          regex_match(entry->d_name, pattern.c_str())) {
        // Don't match
        continue;
      }

      dirList.push_back(fullPath);
    }

    closedir(dirp);
    return 0;
  } catch (...) {
    std::cerr << "Failed to get file list " << path << SYS_OUTPUT_FILE_POS
              << SYS_OUTPUT_ERROR << std::endl;
  }
  return -1;
}

int touch(const std::string &path) {
  struct stat fs;

  memset(&fs, 0, sizeof(fs));
  if (stat(path.c_str(), &fs) == 0) {
    return 0;
  }

  // create the file
  FILE *file = fopen(path.c_str(), "a");
  if (file == NULL) {
    return -1;
  }
  fclose(file);
  return 0;
}

int getFileSize(const char *filePath, u64_t &fileLen) {
  if (filePath == NULL || *filePath == '\0') {
    std::cerr << "invalid filepath" << std::endl;
    return -EINVAL;
  }
  struct stat statBuf;
  memset(&statBuf, 0, sizeof(statBuf));

  int rc = stat(filePath, &statBuf);
  if (rc) {
    std::cerr << "Failed to get stat of " << filePath << "," << errno << ":"
              << strerror(errno) << std::endl;
    return rc;
  }

  if (S_ISDIR(statBuf.st_mode)) {
    std::cerr << filePath << " is directory " << std::endl;
    return -EINVAL;
  }

  fileLen = statBuf.st_size;
  return 0;
}

} //namespace common