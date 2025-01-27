#include <cstdlib>
#include <exception>
#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for to_string, operator+
                  //
#include "../include/Facade.h"
#include "../include/tui/MainDialog.h"
#include <ctime>
#include <iostream>

int main(int argc, char *argv[]) {

  MainDialog tui = MainDialog();
  // if there are no arguments, writes a file in current path
  if (argc == 1) {
    try {
      Facade::getInstance()->overwriteTasker("tasks.json");
      tui.mainMenuDisplay();
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
  }

  std::string command = argv[1];

  if (command == "help" || command == "-h") {
    std::cout << "Help: Available commands are:\n";
    std::cout
        << "  -o <path/name.json> : Overwrite tasker with specified file.\n";
    std::cout << "  help | -h          : Show this help message.\n";
    return 0;
  }

  // ensure there are enough arguments for -o command
  if (command == "-o" && argc > 2) {
    std::string arg = argv[2];
    try {
      Facade::getInstance()->overwriteTasker(arg);
      tui.mainMenuDisplay();
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
  } else if (command != "-o" && argc == 2) {
    // handle case when user provides only the command without enough arguments
    std::cerr << "Error: Missing arguments for command '" << command << "'\n";
    std::cerr << "Use 'help' to get more information.\n";
  } else {
    // handle other unknown commands
    std::cerr << "Error: Unrecognized command '" << command << "'\n";
    std::cerr << "Use 'help' to get more information.\n";
  }

  return 0;
}
