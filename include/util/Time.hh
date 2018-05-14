#ifndef UTIL__TIME_HH
#define UTIL__TIME_HH
#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace MATHUSLA {

namespace util { ///////////////////////////////////////////////////////////////////////////////

namespace time { ///////////////////////////////////////////////////////////////////////////////

static const auto FullFormat = "%Y%m%d_%H%M%S";
static const auto DateFormat = "%Y%m%d";
static const auto TimeFormat = "%H%M%S";

static const auto CURRENT_TIME = std::time(nullptr);

inline const std::string GetString(const char* format=FullFormat) {
  std::stringstream out;
  out << std::put_time(std::gmtime(&CURRENT_TIME), format);
  return out.str();
}

inline const std::string GetDate() { return GetString(DateFormat); }
inline const std::string GetTime() { return GetString(TimeFormat); }

} /* namespace time */ /////////////////////////////////////////////////////////////////////////

} /* namespace util */ /////////////////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__TIME_HH */
