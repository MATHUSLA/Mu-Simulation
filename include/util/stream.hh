/*
 * include/util/stream.hh
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

#ifndef UTIL__STREAM_HH
#define UTIL__STREAM_HH
#pragma once

#include <iostream>
#include <sstream>

namespace MATHUSLA {

namespace util { namespace stream { ////////////////////////////////////////////////////////////

//__Forward Arguments to Stream_________________________________________________________________
template<class Stream, class T>
Stream& forward(Stream& s,
                T&& arg) {
  s << std::forward<T>(arg);
  return s;
}
template<class Stream, class T, class... Args>
Stream& forward(Stream& s,
                T&& arg,
                Args&&... args) {
  return forward(forward(s, arg), args...);
}
//----------------------------------------------------------------------------------------------

} } /* namespace util::stream */ ///////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__STREAM_HH */
