#ifndef MAINDIALOG
#define MAINDIALOG

#include "../Task.h"
#include <array>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>
#include <vector>
#define MAX_TASKS 100
class MainDialog {
public:
  void mainMenuDisplay();

private:
  void handleEvents(ftxui::Component component,
                    ftxui::ScreenInteractive &screen);
  ftxui::Component createRenderer();
  void updateTasksByTag();
  void getMenuEntries();
  ftxui::Color getEntryColor(int taskIndex);
  void refreshTasksAndMenu();

  std::vector<std::shared_ptr<Task>> tasks;
  std::vector<std::string> tags;
  int selectedTag;
  int selected;
  std::array<bool, MAX_TASKS> states;
  ftxui::Component menu;
};

#endif // !MAINDIALOG
