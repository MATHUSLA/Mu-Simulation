/*
 * include/util/string.hh
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

#ifndef UTIL__STRING_HH
#define UTIL__STRING_HH
#pragma once

#include <string>

namespace MATHUSLA {

namespace util { namespace string { ////////////////////////////////////////////////////////////

//__Split String on Delimeters__________________________________________________________________
template <class Range>
void split(const std::string& string,
           Range& tokens,
           const std::string& delimiters=" ") {
  const auto size = string.size();
  std::string::size_type position, previous = 0;
  while (previous <= size) {
    position = string.find_first_of(delimiters, previous);
    if (position == std::string::npos) position = size;
    tokens.emplace_back(string.data() + previous, position - previous);
    previous = position + 1;
  }
}
//----------------------------------------------------------------------------------------------

//__Split on New Lines__________________________________________________________________________
template <class Range>
void splitlines(const std::string& string,
                Range& tokens) {
  split(string, tokens, "\n\r");
}
//----------------------------------------------------------------------------------------------

//__Join Range of Strings on Delimeters_________________________________________________________
template <class Range>
std::string join(const Range& tokens,
                 const std::string& delimeter="") {
  const auto begin = tokens.cbegin();
  const auto end = tokens.cend();
  if (begin == end)
    return "";

  std::string out;
  out.append(*begin);
  std::for_each(begin + 1, end, [&](const auto& string) { out += delimeter + string; });
  return out;
}
//----------------------------------------------------------------------------------------------

//__Remove Leading and Trailing Spaces__________________________________________________________
inline std::string& strip(std::string& string) {
  const auto end = string.end();
  auto forward = string.cbegin();
  while (std::isspace(*forward) && forward != end) ++forward;
  auto reverse = string.crbegin();
  while (std::isspace(*reverse) && reverse.base() != forward) ++reverse;
  return string = std::string(forward, reverse.base());
}
//----------------------------------------------------------------------------------------------

//__Remove Leading and Trailing Spaces__________________________________________________________
inline std::string strip(const std::string& string) {
  auto copy = string;
  return strip(copy);
}
//----------------------------------------------------------------------------------------------

//__String to All Lowercase_____________________________________________________________________
inline std::string& tolower(std::string& string) {
  std::transform(string.cbegin(), string.cend(), string.begin(),
    [](unsigned char c){ return std::tolower(c); });
  return string;
}
//----------------------------------------------------------------------------------------------

//__String to All Lowercase_____________________________________________________________________
inline std::string tolower(const std::string& string) {
  auto copy = string;
  return util::string::tolower(copy);
}
//----------------------------------------------------------------------------------------------

//__String to All Uppercase_____________________________________________________________________
inline std::string& toupper(std::string& string) {
  std::transform(string.cbegin(), string.cend(), string.begin(),
    [](unsigned char c){ return std::toupper(c); });
  return string;
}
//----------------------------------------------------------------------------------------------

//__String to All Uppercase_____________________________________________________________________
inline std::string toupper(const std::string& string) {
  auto copy = string;
  return util::string::toupper(copy);
}
//----------------------------------------------------------------------------------------------

} } /* namespace util::string */ ///////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__STRING_HH */
