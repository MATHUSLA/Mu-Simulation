/* include/util/time.hh
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
