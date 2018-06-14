/*
 * src/util/command_line_parser.cc
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

#include "util/command_line_parser.hh"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>

#ifdef _BSD_SOURCE
#include <sysexits.h>
#else
#define EX_USAGE EXIT_FAILURE
#endif

#include "util/error.hh"

namespace MATHUSLA {

namespace util { namespace cli { ///////////////////////////////////////////////////////////////

    /* TODO: ALTERNATIVE IMPLEMENTATION
    class option {
    public:
      enum class argument_type { none, optional, required };
      enum class name_type { one_char, one_word, multi_word };

      option() {}

      option& short_name(const char name) {
        _short = name;
        return *this;
      }

      option& long_name(const std::string& name) {
        _long = name;
        return *this;
      }

      option& description(const std::string& desc) {
        _desc = desc;
        return *this;
      }

      std::size_t count() const { return _count; }
      const std::string& argument() const { return _argument; }
    private:
      char _short;
      std::string _long;
      std::string _desc;
      std::size_t _count;
      std::string _argument;
    };

    template<typename ...Options>
    size_t parse(char* const argv[], Options&... options) {}
    */

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Find Long Option from Option List___________________________________________________________
option* _find_long_option(const std::string& arg,
                          const option_list& options) {
  size_t count = 0, found = 0, index = 0, length = std::strcspn(arg.c_str(), "=");

  for (const auto& option : options) {
    if (!option->long_name.empty()) {
      size_t full = option->long_name.length();
      if ((length <= full) && !arg.compare(0, length, option->long_name)) {
        if (length == full)
          return option;
        found = index;
        count++;
      }
    }
    index++;
  }

  if (index == options.size()) return {};
  return (count == 1) ? options[found] : options[index];
}
//----------------------------------------------------------------------------------------------

//__Find Short Option from Option List__________________________________________________________
option* _find_short_option(const char arg,
                           const option_list& options) {
  for (const auto& option : options)
    if (option->short_name == arg)
      return option;
  return nullptr;
}
//----------------------------------------------------------------------------------------------

//__Long Option Check___________________________________________________________________________
bool _is_long(const std::string& arg) {
  return arg[0] == '-' && arg[1] == '-' && arg[2];
}
//----------------------------------------------------------------------------------------------

//__Short Option Check__________________________________________________________________________
bool _is_short(const std::string& arg) {
  return arg[0] == '-' && arg[1];
}
//----------------------------------------------------------------------------------------------

//__Option Type Check___________________________________________________________________________
bool _is_on(const option* option,
            const size_t flags) {
  return (option->flags & flags) == flags;
}
//----------------------------------------------------------------------------------------------

//__Auto Help Message___________________________________________________________________________
void _print_help_message(const std::string& argv0,
                         const option* help,
                         const option_list& options) {
  if (!help || !help->count) return;

  std::cout << "\r\n " << help->description << "  " << argv0 << " [args]\n";

  for (const auto& option : options) {
    if (option == help) continue;

    std::cout << "  -" << option->short_name;
    if (!option->long_name.empty())
      std::cout << " --" << option->long_name;
    if (!option->description.empty()) {
      std::cout << std::setw(15 - option->long_name.length()) << " : "
                << option->description;
    }
    std::cout << '\n';
  }
  std::cout << '\n';
  exit(EX_USAGE);
}
//----------------------------------------------------------------------------------------------

//__Auto Error Message__________________________________________________________________________
void _print_error_message(const std::string& argv0,
                          const option_list& options) {
  for (const auto& option : options) {
    error::exit_when(_is_on(option, option::is_short_without_argument | option::required_arguments),
      argv0, ": option -", option->short_name, " requires an argument\n");

    error::exit_when(_is_on(option, option::is_short_with_argument | option::no_arguments),
      argv0, ": option -", option->short_name, " must not have an argument\n");

    error::exit_when(_is_on(option, option::is_long_without_argument | option::required_arguments),
      argv0, ": option --", option->long_name, " requires an argument\n");

    error::exit_when(_is_on(option, option::is_long_with_argument | option::no_arguments),
      argv0, ": option --", option->long_name, " must not have an argument\n");

    error::exit_when((option->count > 1) && !_is_on(option, option::repeatable),
      argv0, ": option -", option->short_name,
             " (--", option->long_name, ") may not be repeated\n");
  }
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Option Constructor__________________________________________________________________________
option::option() : option(0) {}
option::option(const char short_name,
               const std::string& long_name,
               const std::string& description,
               const size_t flags)
    : short_name(short_name), long_name(long_name), description(description),
      flags(flags), count(0), argument(nullptr) {}
//----------------------------------------------------------------------------------------------

//__Commmand Line Option Parser_________________________________________________________________
size_t parse(char* argv[],
             option_list options) {
  if (!options.size()) return 0;

  for (auto& option : options)
    option->reset();

  std::vector<size_t> operand_indicies{};

  size_t operand_count = 1;
  size_t expecting = option::empty;

  option* current = nullptr;
  auto error = new option();

  for (size_t i = 1; argv[i]; ++i) {

    if (expecting != option::empty) {
      if (!_is_short(argv[i]) || !(_is_on(current, option::no_hyphen_arguments))) {
        current->set_argument(argv[i], expecting);
        expecting = option::empty;
        continue;
      } else {
        current->set_argument(nullptr, expecting >> 1);
        expecting = option::empty;
      }
    }

    if (_is_long(argv[i])) {
      char* eq = std::strchr(&argv[i][2], '=');
      current = _find_long_option(&argv[i][2], options);
      if (!current)
        current = error;
      current->count++;

      if (_is_on(current, option::error) && current->long_name.empty()) {
        current->long_name = std::string(&argv[i][2]);
      }

      if (eq) {
        current->set_argument(1 + eq, option::is_long_with_argument);
      } else if (_is_on(current, option::required_arguments)) {
        expecting = option::is_long_with_argument;
      } else {
        current->set_argument(nullptr, option::is_long_without_argument);
      }
    } else if (_is_short(argv[i])) {
      for (size_t j = 1; argv[i][j]; ++j) {
        current = _find_short_option(argv[i][j], options);
        if (!current)
          current = error;

        current->count++;

        if (_is_on(current, option::error)) {
          if (!current->short_name)
            current->short_name = argv[i][j];

          if (argv[i][j+1]) {
            current->set_argument(&argv[i][j+1], option::is_short_with_argument);
          } else {
            current->set_argument(nullptr, option::is_short_without_argument);
          }
          break;
        }

        if (_is_on(current, option::no_arguments)) {
          current->set_argument(nullptr, option::is_short_without_argument);
        } else if (argv[i][j+1]) {
          current->set_argument(&argv[i][j+1], option::is_short_with_argument);
          break;
        } else if (_is_on(current, option::required_arguments)) {
          expecting = option::is_short_with_argument;
        } else {
          current->set_argument(nullptr, option::is_short_without_argument);
        }
      }
    } else {
      argv[operand_count] = argv[i];
      operand_count++;
    }

  }

  if (expecting != option::empty) {
    current->set_argument(nullptr, expecting >> 1);
  }

  // argv[operand_count] = nullptr;

  for (const auto& i : operand_indicies)
    std::cout << argv[i] << " ";
  std::cout << "\n";

  error::exit_when(error->short_name,
    argv[0], ": unrecognised option -", error->short_name, '\n');

  error::exit_when(error->long_name[0],
    argv[0], ": unrecognised option -", error->short_name, '\n');

  _print_error_message(argv[0], options);

  option* help = nullptr;
  for (const auto& option : options)
    if (option->long_name == "help")
      help = option;

  _print_help_message(argv[0], help, options);

  delete error;

  return operand_count;
}
//----------------------------------------------------------------------------------------------

} } /* namespace util::cli */ //////////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */
