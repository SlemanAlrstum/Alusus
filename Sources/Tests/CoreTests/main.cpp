/**
 * @file Tests/CoreTests/main.cpp
 * Contains the program's entry point.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core_tests.h"
#include <catch_runner.hpp>

using namespace Core;

/**
 * @defgroup core_tests Core Tests
 * @brief Contains elements related to Core tests.
 */

// These references are declared to enable easily converting between unicode and ascii.
static std::ostream & Cout = std::cout;
static std::istream & Cin = std::cin;


/**
 * @brief The entry point of the program.
 * @ingroup core_tests
 *
 * Parse the file with the name passed from the command line.
 */
int main(int argCount, char * const args[])
{
  // Validate arguments.
  for (int i = 0; i < argCount; ++i) {
    if (strcmp(args[i], STR("--help")) == 0) {
      Cout << STR("Alusus Core Tests\n"
                  "Version " ALUSUS_VERSION "\n"
                  "Copyright (C) " ALUSUS_RELEASE_YEAR " Sarmad Khalid Abdullah\n\n");
      Cout << STR("Usage: alusus_tests [<options>]\n");
      Cout << STR("\nOptions:\n");
#if defined(USE_LOGS)
      Cout << STR("\t--log  A 3 bit value to control the level of details of the log.\n");
#endif
      Cout << STR("\nUnit Test Specific Options:\n");
      Cout << STR("\t-l, --list  <tests | reporters> [xml]\n");
      Cout << STR("\t-t, --test  <testspec> [<testspec>...]\n");
      Cout << STR("\t-r, --reporter  <reporter name>\n");
      Cout << STR("\t-o, --out  <file name>|<%stream name>\n");
      Cout << STR("\t-s, --success\n");
      Cout << STR("\t-b, --break\n\n");
      Cout << STR("For more detail usage please see: https://github.com/philsquared/Catch/wiki/Command-line\n");
      return 0;
    }
  }

#ifdef USE_LOGS
  // Parse the log option.
  for (Int i = 1; i < argCount-1; i++) {
    if (strcmp(args[i], STR("--log")) == 0) {
      Logger::filter = atoi(args[i+1]);
      break;
    }
  }
#endif

  // Create the args list, skipping non CATCH arguments.
  char **utargs = new char*[argCount];
  int utcount = 0;
  for (Int i = 1; i < argCount; i++) {
    if (strcmp(args[i], STR("--log")) == 0) {
      ++i;
      continue;
    }
    utargs[utcount] = const_cast<char*>(args[i]);
    utcount++;
  }
  // Run the unit tests.
  int ret = Catch::Main(utcount, utargs);
  delete[] utargs;
  return ret;
}
