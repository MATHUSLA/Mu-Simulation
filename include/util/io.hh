#ifndef UTIL__IO_HH
#define UTIL__IO_HH
#pragma once

#include <cstdio>
#include <sys/stat.h>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace MATHUSLA {

namespace util { ///////////////////////////////////////////////////////////////////////////////

namespace io { /////////////////////////////////////////////////////////////////////////////////

//__Create Directory____________________________________________________________________________
inline int create_directory(const std::string& dir) {
  #if defined(_WIN32)
    return _mkdir(dir.c_str());
  #else
    return mkdir(dir.c_str(), 0733);
  #endif
}
//----------------------------------------------------------------------------------------------

//__Check if Path Exists________________________________________________________________________
inline bool path_exists(const std::string& path) {
  struct stat info;
  return !stat(path.c_str(), &info);
}
//----------------------------------------------------------------------------------------------

//__Remove File_________________________________________________________________________________
inline bool remove_file(const std::string& path) {
  return !std::remove(path.c_str());
}
//----------------------------------------------------------------------------------------------

//__Rename File_________________________________________________________________________________
inline bool rename_file(const std::string& path, const std::string& new_path) {
  return !std::rename(path.c_str(), new_path.c_str());
}
//----------------------------------------------------------------------------------------------

} /* namespace io */ ///////////////////////////////////////////////////////////////////////////

} /* namespace util */ /////////////////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__IO_HH */
