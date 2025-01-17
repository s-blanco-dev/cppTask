// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/color.hpp" // for Color, Color::Default, Color::GrayDark, Color::White
#include <cstdlib>
#include <exception>
#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for to_string, operator+
                  //
#include "../include/Facade.h"
#include "../include/Priority.h"
#include "../include/ScreenElements.h"
#include <ctime>
#include <iostream>

int main(int argc, char *argv[]) {
  // If there are no arguments, we show help or default behavior
  if (argc == 1) {
    // Default behavior when no arguments are passed
    try {
      Facade::getInstance()->overwriteTasker("tasks.json");
      Facade::getInstance()->tuiViewTasks();
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
  }

  // Process the command
  std::string command = argv[1];

  // Check if the user requested help
  if (command == "help" || command == "-h") {
    std::cout << "Help: Available commands are:\n";
    std::cout
        << "  -o <path/name.json> : Overwrite tasker with specified file.\n";
    std::cout << "  help | -h          : Show this help message.\n";
    return 0;
  }

  // Ensure there are enough arguments for -o command
  if (command == "-o" && argc > 2) {
    std::string arg = argv[2];
    try {
      Facade::getInstance()->overwriteTasker(arg);
      Facade::getInstance()->tuiViewTasks();
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
  } else if (command != "-o" && argc == 2) {
    // Handle case when user provides only the command without enough arguments
    std::cerr << "Error: Missing arguments for command '" << command << "'\n";
    std::cerr << "Use 'help' to get more information.\n";
  } else {
    // Handle other unknown commands
    std::cerr << "Error: Unrecognized command '" << command << "'\n";
    std::cerr << "Use 'help' to get more information.\n";
  }

  return 0;
}
