/* include/util/error.hh
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

#ifndef UTIL__ERROR_HH
#define UTIL__ERROR_HH
#pragma once

#include <iostream>

namespace MATHUSLA {

namespace util { ///////////////////////////////////////////////////////////////////////////////

namespace error { //////////////////////////////////////////////////////////////////////////////

namespace detail { /////////////////////////////////////////////////////////////////////////////

//__Forward Arguments to std::cout______________________________________________________________
template<class T>
void std_cout_forward(T&& arg) {
  std::cout << std::forward<T>(arg);
}
template<class T, class... Args>
void std_cout_forward(T&& arg, Args&&... args) {
  std_cout_forward(arg);
  std_cout_forward(args...);
}
//----------------------------------------------------------------------------------------------

} /* namespace detail */ ///////////////////////////////////////////////////////////////////////

//__Boolean Exit Convenience Functions__________________________________________________________
template<class... Args>
void exit_when(bool value, int code, Args&&... msgs) {
  if (value) {
    detail::std_cout_forward(msgs...);
    exit(code);
  }
}
template<class... Args>
void exit_when(bool value, Args&&... msgs) {
  exit_when(value, EXIT_FAILURE, msgs...);
}
//----------------------------------------------------------------------------------------------

//__Exit Convenience Functions__________________________________________________________________
template<class... Args>
void exit(int code, Args&&... msgs) { exit_when(true, code, msgs...); }
template<class... Args>
void exit(Args&&... msgs) { exit_when(true, EXIT_FAILURE, msgs...); }
//----------------------------------------------------------------------------------------------

} /* namespace error */ ////////////////////////////////////////////////////////////////////////

} /* namespace util */ /////////////////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__ERROR_HH */
