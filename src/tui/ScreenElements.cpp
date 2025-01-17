#include "ftxui/component/component.hpp"      // for Button, Renderer, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp" // for operator|, text, Element, hbox, separator, size, vbox, border, frame, vscroll_indicator, HEIGHT, LESS_THAN
#include "ftxui/screen/color.hpp" // for Color, Color::Default, Color::GrayDark, Color::White
#include <cstdlib>
#include <exception>
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
  auto descriptionInputBox = Input(&inputText, "Description");

  std::vector<std::string> entries = {"High", "Medium", "Low"};
  int selected = 0;

  auto style = ButtonOption::Ascii();
  auto saveBtn = Button(
      "Save",
      [&] {
        if (!inputText.empty()) {
          Facade::getInstance()->newTask(
              inputText, Priority::fromString(entries[selected]));
          screen.Exit();
        }
      },
      style);
  auto cancelBtn = Button("Cancel", [&] { screen.Exit(); }, style);

  auto radiobox = Radiobox(&entries, &selected);
  auto main_container = Container::Horizontal({
      descriptionInputBox,
      saveBtn,
      cancelBtn,
      radiobox,
  });

  auto component = Renderer(main_container, [&] {
    return hbox({
               vbox({text("New Task:") | bold, separatorDashed(),
                     descriptionInputBox->Render() | frame |
                         size(ftxui::HEIGHT, ftxui::EQUAL, 3) | flex_grow,
                     separator(),
                     hbox({
                         saveBtn->Render() | frame | color(Color::Green),
                         cancelBtn->Render() | frame | color(Color::Red),
                     }) | size(HEIGHT, ftxui::EQUAL, 1)}) |
                   xflex,
               separator(),
               vbox({
                   text("Priority:") | center | bold,
                   radiobox->Render() | vscroll_indicator | frame,
               }),
           }) |
           flex | border;
  });
  screen.Loop(component);
}

void ScreenElements::viewTasks() {
  using namespace ftxui;

  auto tasks = Facade::getInstance()->getTasks();
  int selected = 0;

  // function to generate menu entries
  // useful to call when it needs to update or reload entries
  auto getMenuEntries = [&]() -> std::vector<std::string> {
    std::vector<std::string> entries;
    for (const auto &task : tasks) {
      std::string completeCheck = task->isCompleted() ? "[*]" : "[ ]";
      entries.push_back(format("{} {}", completeCheck, task->getDescription()));
    }
    if (tasks.empty()) {
      entries.push_back("Press 'n' to create a new task.");
    }
    return entries;
  };

  std::vector<std::string> entries = getMenuEntries();
  auto menu = Menu(&entries, &selected);

  auto renderer = Renderer(menu, [&] {
    // list of tasks
    auto left_menu = vbox({
        text("To-Do:") | bold | color(Color::Red),
        separator(),
        menu->Render() | vscroll_indicator | frame | flex,
    });

    // details of the selected task (if tasks exist)
    auto right_menu =
        // if no tasks exist --> print 'no task' | else: print details
        tasks.empty()
            ? vbox({text("No tasks to display") | bold})
            : vbox({text("Task Details:") | bold | color(Color::Yellow),
                    separator(),
                    hbox({text("ID: "),
                          text(std::to_string(tasks[selected]->getId()))}),
                    separatorDashed(),
                    hbox({text("Priority: ") | bold | color(Color::Cyan),
                          text(Priority::toString(
                              tasks[selected]->getPriority()))}),
                    separatorDashed(),
                    hbox({text("Created: ") | bold | color(Color::Green),
                          paragraph(tasks[selected]->getRelativeTimeMessage()) |
                              flex}),
                    separatorDashed(),
                    hbox({text("Completed: ") | bold | color(Color::Blue),
                          text(tasks[selected]->isCompleted() ? "Yes" : "No")}),
                    separatorDashed(),
                    hbox({
                        text("Progress: ") | bold | color(Color::Magenta),
                        gaugeRight(tasks[selected]->getProgress() * 0.01) |
                            color(Color::Magenta),
                        separatorEmpty(),
                        text(std::to_string(tasks[selected]->getProgress()) +
                             "\%") |
                            bold,
                    })}) |
                  frame | flex;

    // combine left and right menus
    return vbox({hbox({
                     left_menu | xflex,
                     separator(),
                     right_menu,
                 }) | yflex,
                 separator(),
                 text("space: toggle completed, r: remove, n: new task, q: "
                      "quit") |
                     frame,
                 text("+/- increase/decrease progress")}) |
           border;
  });

  auto screen = ScreenInteractive::Fullscreen();

  auto refreshTasksAndMenu = [&]() {
    tasks = Facade::getInstance()->getTasks(); // refresh tasks
    entries = getMenuEntries();                // refresh menu entries
    selected = std::min(selected, (int)tasks.size() - 1);
  };

  auto component = CatchEvent(renderer, [&](Event event) {
    std::string eventChar = event.character();

    // new task
    if (eventChar == "n") {
      Facade::getInstance()->tuiCreateTask();
      refreshTasksAndMenu();
      return true; // refresh screen
      // quit program
    } else if (eventChar == "q") {
      screen.Exit();
      return true;
      // mark as completed
    } else if (eventChar == " " && !tasks.empty()) {
      Facade::getInstance()->toggleTaskCompleted(tasks[selected]);
      entries = getMenuEntries();
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
      entries = getMenuEntries();
      return true;
      // decrease task progress
    } else if (eventChar == "-" && !tasks.empty()) {
      int newProgress = tasks[selected]->getProgress() - 10;
      Facade::getInstance()->setTaskProgress(tasks[selected], newProgress);
      tasks[selected]->setProgress(newProgress);
      entries = getMenuEntries();
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
