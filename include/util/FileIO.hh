#ifndef UTIL_FILEIO_HH
#define UTIL_FILEIO_HH
#pragma once

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace MATHUSLA {

namespace IO {

inline int create_directory(const std::string& dir) {
  #if defined(_WIN32)
    return _mkdir(dir.c_str());
  #else
    return mkdir(dir.c_str(), 0733);
  #endif
}

inline bool file_exists(const std::string& path) {
  struct stat info;
  return !stat(path.c_str(), &info);
}

inline bool remove_file(const std::string& path) {
  return !remove(path.c_str());
}

inline bool rename_file(const std::string& path, const std::string& new_path) {
  return !rename(path.c_str(), new_path.c_str());
}

} /* namespace IO */

} /* namespace MATHUSLA */

#endif /* UTIL_FILEIO_HH */
