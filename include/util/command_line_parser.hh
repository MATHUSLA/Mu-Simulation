/* include/util/command_line_parser.hh
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

#ifndef UTIL__COMMAND_LINE_PARSER_HH
#define UTIL__COMMAND_LINE_PARSER_HH
#pragma once

#include <string>
#include <vector>

namespace MATHUSLA {

namespace util { ///////////////////////////////////////////////////////////////////////////////

namespace cli { ////////////////////////////////////////////////////////////////////////////////

//__Command Line Option_________________________________________________________________________
// TODO: needs work
struct option {
  enum : size_t {
    empty               = 0x000u,
    optional_arguments  = 0x001u,
    no_arguments        = 0x002u,
    required_arguments  = 0x004u,
    no_hyphen_arguments = 0x008u,

    repeatable                = 0x010u,
    is_short_without_argument = 0x020u,
    is_short_with_argument    = 0x040u,
    is_long_without_argument  = 0x080u,
    is_long_with_argument     = 0x100u,

    error = 0x200u
  };

  option();
  option(char short_name,
         const std::string& long_name="",
         const std::string& description="",
         size_t flags=empty);

  void reset() {
    count = 0;
    argument = nullptr;
  }

  void set_argument(char* arg, size_t flag) {
    argument = arg;
    flags |= flag;
  }

  bool operator==(const option& rhs) {
    return short_name  == rhs.short_name
        && long_name   == rhs.long_name
        && description == rhs.description
        && flags       == rhs.flags
        && count       == rhs.count
        && argument    == rhs.argument;
  }

  char              short_name;
  std::string       long_name;
  const std::string description;
  size_t            flags;
  size_t            count;
  const char*       argument;
};
//----------------------------------------------------------------------------------------------

//__Command Line Option List____________________________________________________________________
using option_list = std::vector<option*>;
//----------------------------------------------------------------------------------------------

//__Commmand Line Option Parser_________________________________________________________________
size_t parse(char* argv[], option_list options);
//----------------------------------------------------------------------------------------------

} /* namespace cli */ //////////////////////////////////////////////////////////////////////////

} /* namespace util */ /////////////////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__COMMAND_LINE_PARSER_HH */
