#include "util/CommandLineParser.hh"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>

#ifdef _WIN32
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

#ifdef _BSD_SOURCE
#include <sysexits.h>
#else
#define EX_USAGE EXIT_FAILURE
#endif

namespace MATHUSLA { namespace MU {

CommandLineOption::CommandLineOption() : CommandLineOption(0) {}

CommandLineOption::CommandLineOption(char short_name,
                                     const char* long_name,
                                     const char* description,
                                     size_t flags)
    : short_name(short_name), long_name(long_name), description(description),
      flags(flags), count(0), argument(nullptr) {}

static CommandLineOption* _find_long_option(const char* arg,
                                            CommandLineOptionList options) {
  size_t count = 0,
         found = 0,
         index = 0,
         length = strcspn(arg, "=");

  for (const auto& option : options) {
    if (option->long_name) {
      size_t full = strlen(option->long_name);
      if ((length <= full) && !strncasecmp(option->long_name, arg, length)) {
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

static CommandLineOption* _find_short_option(char arg,
                                             CommandLineOptionList options) {
  for (const auto& option : options)
    if (option->short_name == arg)
      return option;
  return nullptr;
}

static bool _is_long(const char* arg) {
  return arg[0] == '-' && arg[1] == '-' && arg[2];
}

static bool _is_short(const char* arg) {
  return arg[0] == '-' && arg[1];
}

static bool _is_on(CommandLineOption* option, size_t flags) {
  return (option->flags & flags) == flags;
}

static void _print_help_message(const char* argv0,
                                CommandLineOption* help,
                                CommandLineOptionList options) {
  if (!help || !help->count) return;

  std::cout << "\r\n " << help->description << "  " << argv0 << " [args]\n";

  for (const auto& option : options) {
    if (option == help) continue;
    std::cout << "  -" << option->short_name;
    if (option->long_name)
      std::cout << " --" << option->long_name;
    if (option->description) {
      std::cout << std::setw(15 - strlen(option->long_name)) << " : "
                << option->description;
    }
    std::cout << '\n';
  }
  std::cout << '\n';
  exit(EX_USAGE);
}

static void _print_error_message(const char* argv0, CommandLineOptionList options) {
  constexpr auto NoArguments            = CommandLineOption::NoArguments;
  constexpr auto RequiredArguments      = CommandLineOption::RequiredArguments;
  constexpr auto Repeatable             = CommandLineOption::Repeatable;
  constexpr auto IsShortWithoutArgument = CommandLineOption::IsShortWithoutArgument;
  constexpr auto IsShortWithArgument    = CommandLineOption::IsShortWithArgument;
  constexpr auto IsLongWithoutArgument  = CommandLineOption::IsLongWithoutArgument;
  constexpr auto IsLongWithArgument     = CommandLineOption::IsLongWithArgument;

  for (auto& option : options) {
    if (_is_on(option, IsShortWithoutArgument | RequiredArguments)) {
      std::cout << argv0
                << ": option -" << option->short_name
                << " requires an argument\n";
      exit(EX_USAGE);
    } else if (_is_on(option, IsShortWithArgument | NoArguments)) {
      std::cout << argv0
                << ": option -" << option->short_name
                << " must not have an argument\n";
      exit(EX_USAGE);
    } else if (_is_on(option, IsLongWithoutArgument | RequiredArguments)) {
      std::cout << argv0
                << ": option --" << option->long_name
                << " requires an argument\n";
      exit(EX_USAGE);
    } else if (_is_on(option, IsLongWithArgument | NoArguments)) {
      std::cout << argv0
                << ": option --" << option->long_name
                << " must not have an argument\n";
      exit(EX_USAGE);
    } else if ((option->count > 1) && !(option->flags & Repeatable)) {
      std::cout << argv0
                << ": option -" << option->short_name
                << " (--" << option->long_name
                << ") may not be repeated\n";
      exit(EX_USAGE);
    }
  }
}

size_t CommandLineParser::parse(char* argv[],
                                std::initializer_list<CommandLineOption*> options) {

  if (!options.size()) return 0;

  constexpr auto Empty                  = CommandLineOption::Empty;
  constexpr auto NoArguments            = CommandLineOption::NoArguments;
  constexpr auto RequiredArguments      = CommandLineOption::RequiredArguments;
  constexpr auto NoHyphenArguments      = CommandLineOption::NoHyphenArguments;
  constexpr auto IsShortWithoutArgument = CommandLineOption::IsShortWithoutArgument;
  constexpr auto IsShortWithArgument    = CommandLineOption::IsShortWithArgument;
  constexpr auto IsLongWithoutArgument  = CommandLineOption::IsLongWithoutArgument;
  constexpr auto IsLongWithArgument     = CommandLineOption::IsLongWithArgument;
  constexpr auto Error                  = CommandLineOption::Error;

  for (auto option : options)
    option->reset();

  size_t operand_count = 1;
  size_t expecting = Empty;

  CommandLineOption* current = nullptr;
  auto error = new CommandLineOption();

  for (size_t i = 1; argv[i]; ++i) {
    if (expecting != Empty) {
      if (!_is_short(argv[i]) || !(_is_on(current, NoHyphenArguments))) {
        current->set_argument(argv[i], expecting);
        expecting = Empty;
        continue;
      } else {
        current->set_argument(nullptr, expecting >> 1);
        expecting = Empty;
      }
    }

    if (_is_long(argv[i])) {
      char* eq = strchr(&argv[i][2], '=');
      current = _find_long_option(&argv[i][2], options);
      if (!current)
        current = error;
      current->count++;

      if (_is_on(current, Error) && !current->long_name) {
        current->long_name = &argv[i][2];
      }

      if (eq) {
        current->set_argument(1 + eq, IsLongWithArgument);
      } else if (_is_on(current, RequiredArguments)) {
        expecting = IsLongWithArgument;
      } else {
        current->set_argument(nullptr, IsLongWithoutArgument);
      }
    } else if (_is_short(argv[i])) {
      for (size_t j = 1; argv[i][j]; ++j) {
        current = _find_short_option(argv[i][j], options);
        if (!current)
          current = error;

        current->count++;

        if (_is_on(current, Error)) {
          if (!current->short_name)
            current->short_name = argv[i][j];

          if (argv[i][j+1]) {
            current->set_argument(&argv[i][j+1], IsShortWithArgument);
          } else {
            current->set_argument(nullptr, IsShortWithoutArgument);
          }
          break;
        }

        if (_is_on(current, NoArguments)) {
          current->set_argument(nullptr, IsShortWithoutArgument);
        } else if (argv[i][j+1]) {
          current->set_argument(&argv[i][j+1], IsShortWithArgument);
          break;
        } else if (_is_on(current, RequiredArguments)) {
          expecting = IsShortWithArgument;
        } else {
          current->set_argument(nullptr, IsShortWithoutArgument);
        }
      }
    } else {
      argv[operand_count] = argv[i];
      operand_count++;
    }
  }

  if (expecting != Empty) {
    current->set_argument(nullptr, expecting >> 1);
  }

   argv[operand_count] = nullptr;

  if (error->short_name) {
    std::cout << argv[0] << ": unrecognised option -" << error->short_name << '\n';
    exit(EX_USAGE);
  }

  if (error->long_name[0]) {
    std::cout << argv[0] << ": unrecognised option -" << error->short_name << '\n';
    exit(EX_USAGE);
  }

  _print_error_message(argv[0], options);

  CommandLineOption* help = nullptr;
  for (const auto& option : options)
    if (!strncmp(option->long_name, "help", 4))
      help = option;

  _print_help_message(argv[0], help, options);

  delete error;

  return operand_count;
}

} } /* namespace MATHUSLA::MU */
