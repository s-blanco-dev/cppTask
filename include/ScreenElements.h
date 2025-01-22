#ifndef SCREENELEMENTS
#define SCREENELEMENTS

#include "Priority.h"
#include "Task.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>
#include <vector>
class ScreenElements {
public:
  static void
  removeTasksAccordingToPriority(Priority::Level prior,
                                 std::vector<std::shared_ptr<Task>> &tasks);
  static void newTaskDialog(ftxui::ScreenInteractive &screen);
  static void viewTasks();
  static bool confirmDialog(ftxui::ScreenInteractive &screen,
                            const std::string &message);
  static void editTaskDialog(ftxui::ScreenInteractive &screen,
                             std::shared_ptr<Task> task);

  static void taskDialog(ftxui::ScreenInteractive &screen, bool isEdit,
                         std::shared_ptr<Task> task = nullptr);
};

#endif // !SCREENELEMENTS
