#include "ftxui/component/component.hpp"      // for Button, Renderer, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp" // for operator|, text, Element, hbox, separator, size, vbox, border, frame, vscroll_indicator, HEIGHT, LESS_THAN
#include "ftxui/screen/color.hpp" // for Color, Color::Default, Color::GrayDark, Color::White
#include <complex>
#include <cstdlib>
#include <exception>
#include <execution>
#include <format>
#include <ftxui/component/event.hpp>
#include <ftxui/component/task.hpp>
#include <ftxui/dom/deprecated.hpp>
#include <ftxui/dom/table.hpp> // for Table, TableSelection
#include <ftxui/screen/screen.hpp>
#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for to_string, operator+
                  //
#include "../../include/Facade.h"
#include "../../include/Priority.h"
#include "../../include/ScreenElements.h"
#include "../../include/Task.h"
#include <ctime>
#include <vector>

// This code is bad, but I don't care
// I may refactor this in the future
void ScreenElements::newTaskDialog(ftxui::ScreenInteractive &screen) {
  using namespace ftxui;

  std::string inputText;
  std::string dueDateText;
  auto descriptionInputBox = Input(&inputText, "Description");
  auto dueDateInputBox = Input(&dueDateText, "DD-MM-YYYY");

  std::vector<std::string> entries = {"High", "Medium", "Low"};
  int selected = 0;

  auto style = ButtonOption::Ascii();
  auto saveBtn = Button(
      "Save",
      [&] {
        if (!inputText.empty() && !dueDateText.empty()) {
          Facade::getInstance()->newTask(
              inputText, Priority::fromString(entries[selected]), dueDateText);
          screen.Exit();
        }
      },
      style);
  auto cancelBtn = Button("Cancel", [&] { screen.Exit(); }, style);

  auto radiobox = Radiobox(&entries, &selected);
  auto main_container = Container::Vertical({
      descriptionInputBox,
      saveBtn,
      cancelBtn,
      radiobox,
      dueDateInputBox,
  });

  auto component = Renderer(main_container, [&] {
    auto descriptionWindow =
        window(text("New Task") | bold | center,
               vbox({descriptionInputBox->Render() | frame |
                         size(ftxui::HEIGHT, ftxui::EQUAL, 3) | flex_grow,
                     separator(),
                     hbox({
                         saveBtn->Render() | frame | color(Color::Green),
                         cancelBtn->Render() | frame | color(Color::Red),
                     }) | size(HEIGHT, ftxui::EQUAL, 1)}));

    auto priorityWindow =
        window(text("Priority") | bold | center,
               vbox({radiobox->Render() | vscroll_indicator | frame}));

    auto dueDateWindow = window(text("Due Date") | bold | center,
                                vbox({dueDateInputBox->Render() | frame}));

    return hbox({descriptionWindow | flex, vbox({
                                               priorityWindow | xflex,
                                               dueDateWindow | xflex,
                                           })});
  });

  screen.Loop(component);
}

void ScreenElements::viewTasks() {
  using namespace ftxui;

  auto tasks = Facade::getInstance()->getTasks();
  int selected = 0;

  // Define a fixed-size array for task states (adjust size as needed)
  const size_t max_tasks = 100;
  std::array<bool, max_tasks> states = {};

  auto menu = Container::Vertical({}, &selected);

  auto getEntryColor = [&](int taskIndex) -> Color {
    if (tasks[taskIndex]->isCompleted()) {
      return Color::GrayDark;
    }
    if (tasks[taskIndex]->getPriority() == Priority::Level::High) {
      return Color::Red;
    } else if (tasks[taskIndex]->getPriority() == Priority::Level::Medium) {
      return Color::Orange1;
    }
    return Color::Wheat1;
  };

  auto getMenuEntries = [&]() {
    menu->DetachAllChildren();
    for (size_t i = 0; i < tasks.size(); ++i) {
      states[i] = tasks[i]->isCompleted();
      menu->Add(Checkbox(format("{}", tasks[i]->getDescription()), &states[i]) |
                color(getEntryColor(i)));
    }
    if (tasks.empty()) {
      menu->Add(MenuEntry("Press 'n' to create a new task."));
    }
  };

  getMenuEntries();

  auto renderer = Renderer(menu, [&] {
    // list of tasks inside a window
    auto left_menu =
        window(text("Tasks") | bold | center,
               vbox({
                   menu->Render() | vscroll_indicator | frame | flex,
               }));

    // details of the selected task (if tasks exist)
    auto right_menu =
        // if no tasks exist --> print 'no task' | else: print details
        tasks.empty()
            ? window(text("Task Details") | bold | center,
                     vbox({text("No tasks to display") | bold}))
            : window(
                  text("Task Details") | bold | center,
                  vbox({
                      hbox({text("ID: "),
                            text(std::to_string(tasks[selected]->getId()))}),
                      separatorDashed(),
                      hbox({
                          text("Priority: ") | bold | color(Color::Red),
                          text(Priority::toString(
                              tasks[selected]->getPriority())),
                      }),
                      separatorDashed(),
                      text("Created: ") | bold | color(Color::Yellow),
                      vbox({
                          paragraph(tasks[selected]->getAbsoluteTimeMessage()) |
                              flex,
                          paragraph("(" +
                                    tasks[selected]->getRelativeTimeMessage() +
                                    ")") |
                              flex,
                      }),
                      separatorDashed(),
                      text("Due date: ") | bold | color(Color::Blue),
                      vbox({
                          paragraph(tasks[selected]->getFullDueDate()) | flex,
                          paragraph("(" +
                                    tasks[selected]->getRelativeDueDate() +
                                    ")") |
                              flex,
                      }),
                      separatorDashed(),

                  }) | frame |
                      flex); // combine left and right menus

    // progress bar or "No task selected" message
    auto progress_section =
        tasks.empty()
            ? text("No task selected")
            : hbox({
                  gaugeRight(tasks[selected]->getProgress() * 0.01) |
                      color(Color::Green),
                  separatorEmpty(),
                  text(std::to_string(tasks[selected]->getProgress()) + "\%") |
                      bold,
              });

    return vbox({
        hbox({
            vbox({left_menu | yflex,
                  window(text("Progress") | bold, progress_section)}) |
                xflex,
            right_menu,
        }) | yflex,
        window(text("Navigation") | bold | center,
               vbox({
                   text("space: toggle completed, r: remove, n: new task, q: "
                        "quit") |
                       center,
                   text("+/- increase/decrease progress") | center,
               })),
    });
  });

  auto screen = ScreenInteractive::Fullscreen();

  auto refreshTasksAndMenu = [&]() {
    tasks = Facade::getInstance()->getTasks(); // refresh tasks
    getMenuEntries();                          // refresh menu entries
    selected = std::min(selected, static_cast<int>(tasks.size()) - 1);
  };

  auto component = CatchEvent(renderer, [&](Event event) {
    std::string eventChar = event.character();

    // new task
    if (eventChar == "n") {
      Facade::getInstance()->tuiCreateTask();
      refreshTasksAndMenu();
      selected = tasks.size() - 1; // Select the newly created task
      return true;                 // refresh screen
      // quit program
    } else if (eventChar == "q") {
      screen.Exit();
      return true;
      // mark as completed
    } else if (eventChar == " " && !tasks.empty()) {
      Facade::getInstance()->toggleTaskCompleted(tasks[selected]);
      states[selected] = tasks[selected]->isCompleted();
      getMenuEntries();
      return true;
      // remove task
    } else if (eventChar == "r" && !tasks.empty()) {
      auto secondScreen = ScreenInteractive::Fullscreen();
      if (confirmDialog(secondScreen, "Remove task?")) {
        Facade::getInstance()->removeTask(tasks[selected]);
        refreshTasksAndMenu();
        return true;
      }
      // increase task progress
    } else if (eventChar == "+" && !tasks.empty()) {
      int newProgress = tasks[selected]->getProgress() + 10;
      Facade::getInstance()->setTaskProgress(tasks[selected], newProgress);
      tasks[selected]->setProgress(newProgress);
      getMenuEntries();
      return true;
      // decrease task progress
    } else if (eventChar == "-" && !tasks.empty()) {
      int newProgress = tasks[selected]->getProgress() - 10;
      Facade::getInstance()->setTaskProgress(tasks[selected], newProgress);
      tasks[selected]->setProgress(newProgress);
      getMenuEntries();
      return true;
    }

    return false;
  });

  screen.Loop(component);
  system("clear");
}

bool ScreenElements::confirmDialog(ftxui::ScreenInteractive &screen,
                                   const std::string &message) {
  using namespace ftxui;

  // State variables
  bool result = false;
  auto style = ButtonOption::Ascii();

  // Buttons
  auto confirm_button = Button(
      "Confirm",
      [&] {
        result = true;
        screen.Exit();
      },
      style);
  auto cancel_button = Button(
      "Cancel",
      [&] {
        result = false;
        screen.Exit();
      },
      style);

  // Container for buttons
  auto buttons = Container::Horizontal({
      confirm_button,
      cancel_button,
  });

  auto dialog_renderer = Renderer(buttons, [&] {
    return vbox({
               text(message) | bold | center,
               separator(),
               hbox({
                   confirm_button->Render() | color(Color::Green),
                   cancel_button->Render() | color(Color::Red),
               }) | center,
           }) |
           border | center;
  });

  // Event loop
  screen.Loop(dialog_renderer);

  return result;
}

void ScreenElements::removeTasksAccordingToPriority(
    Priority::Level prior, std::vector<std::shared_ptr<Task>> &tasks) {
  for (int i = 0; i < tasks.size(); i++) {
    if (tasks[i]->getPriority() != prior) {
      tasks.erase(tasks.begin() + i);
    }
  }
}
