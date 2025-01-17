#ifndef SCREENELEMENTS
#define SCREENELEMENTS

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
class ScreenElements {
public:
  static void newTaskDialog(ftxui::ScreenInteractive &screen);
  static void viewTasks();
  static bool confirmDialog(ftxui::ScreenInteractive &screen,
                            const std::string &message);
};

#endif // !SCREENELEMENTS
