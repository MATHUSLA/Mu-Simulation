#ifndef UTIL_FILEIO_HH
#define UTIL_FILEIO_HH
#pragma once

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace MATHUSLA { namespace MU {

namespace IO {

inline int create_directory(const std::string& dir) {
  #if defined(_WIN32)
    return _mkdir(dir.c_str());
  #else
    return mkdir(dir.c_str(), 0733);
  #endif
}

} /* namespace IO */

} } /* namespace MATHUSLA::MU */

#endif /* UTIL_FILEIO_HH */