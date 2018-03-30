#ifndef UTIL_TIME_HH
#define UTIL_TIME_HH
#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace MATHUSLA { namespace MU {

namespace Time {

static const char* FullFormat = "%Y%m%d_%H%M%S";
static const char* DateFormat = "%Y%m%d";
static const char* TimeFormat = "%H%M%S";

static const auto TIME = std::time(nullptr);

inline const std::string GetString(const char* format=FullFormat) {
  std::stringstream out;
  out << std::put_time(std::gmtime(&TIME), format);
  return out.str();
}

inline const std::string GetDate() { return GetString(DateFormat); }
inline const std::string GetTime() { return GetString(TimeFormat); }

} /* namespace Time */

} } /* namespace MATHUSLA::MU */

#endif /* UTIL_TIME_HH */
