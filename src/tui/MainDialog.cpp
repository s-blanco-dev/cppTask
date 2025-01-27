#include "../../include/tui/MainDialog.h"
#include "../../include/Facade.h"
#include "../../include/Priority.h"
#include "../../include/Task.h"
#include "../../include/tui/UserPrompts.h"
#include "ftxui/component/component.hpp"      // for Button, Renderer, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include <cstdlib>
#include <ctime>
#include <format>
#include <ftxui/component/event.hpp>
#include <ftxui/component/task.hpp>
#include <ftxui/dom/deprecated.hpp>
#include <ftxui/dom/table.hpp> // for Table, TableSelection
#include <ftxui/screen/screen.hpp>
#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for to_string, operator+
#include <vector>

//----------------------------------------
void MainDialog::mainMenuDisplay() {
  using namespace ftxui;

  // initialize variables
  this->tags = Facade::getInstance()->getAllTags();
  this->tasks = Facade::getInstance()->getTasks();
  this->selectedTag = 0;
  this->selected = 0;
  this->states = {};
  this->menu = Container::Vertical({}, &selected);

  getMenuEntries();
  updateTasksByTag();

  auto renderer = createRenderer();
  auto screen = ScreenInteractive::Fullscreen();

  handleEvents(renderer, screen);
  system("clear");
}

//----------------------------------------

void MainDialog::getMenuEntries() {
  menu->DetachAllChildren();
  if (tasks.empty()) {
    menu->Add(ftxui::MenuEntry("Press 'n' to create a new task."));
  } else {
    for (size_t i = 0; i < tasks.size(); ++i) {
      states[i] = tasks[i]->isCompleted();
      menu->Add(ftxui::Checkbox(format("{}", tasks[i]->getDescription()),
                                &states[i]) |
                ftxui::color(getEntryColor(i)));
    }
  }
}

//----------------------------------------

void MainDialog::updateTasksByTag() {
  tags = Facade::getInstance()->getAllTags();
  tasks = Facade::getInstance()->getTasksByTag(tags[selectedTag]);
  getMenuEntries();
}

//----------------------------------------

ftxui::Component MainDialog::createRenderer() {
  auto renderer = ftxui::Renderer(menu, [&] {
    // Determine the current tag to display
    std::string currentTagText = "none";
    if (!tags.empty()) {
      currentTagText = tags[selectedTag];
    }

    // List of tasks inside a window
    auto left_menu =
        ftxui::window(ftxui::text("Tasks") | ftxui::bold | ftxui::center,
                      ftxui::vbox({
                          ftxui::text("Current tag: " + currentTagText) |
                              ftxui::bold | ftxui::frame,
                          ftxui::separator(),
                          menu->Render() | ftxui::vscroll_indicator |
                              ftxui::frame | ftxui::flex,
                      }));

    // Details of the selected task (if tasks exist)
    auto right_menu =
        tasks.empty()
            ? ftxui::window(ftxui::text("Task Details") | ftxui::bold |
                                ftxui::center,
                            ftxui::vbox({ftxui::text("No tasks to display") |
                                         ftxui::bold}))
            : ftxui::window(
                  ftxui::text("Task Details") | ftxui::bold | ftxui::center,
                  ftxui::vbox({
                      ftxui::hbox({ftxui::text("Priority: ") | ftxui::bold |
                                       ftxui::color(ftxui::Color::Red),
                                   ftxui::text(Priority::toString(
                                       tasks[selected]->getPriority()))}),
                      ftxui::separatorDashed(),
                      ftxui::text("Created: ") | ftxui::bold |
                          ftxui::color(ftxui::Color::Yellow),
                      ftxui::vbox({
                          ftxui::paragraph(
                              tasks[selected]->getAbsoluteTimeMessage()) |
                              ftxui::flex,
                          ftxui::paragraph(
                              "(" + tasks[selected]->getRelativeTimeMessage() +
                              ")") |
                              ftxui::flex,
                      }),
                      ftxui::separatorDashed(),
                      ftxui::text("Due date: ") | ftxui::bold |
                          ftxui::color(ftxui::Color::Blue),
                      ftxui::vbox({
                          ftxui::paragraph(tasks[selected]->getFullDueDate()) |
                              ftxui::flex,
                          ftxui::paragraph(
                              "(" + tasks[selected]->getRelativeDueDate() +
                              ")") |
                              ftxui::flex,
                      }),
                  }) | ftxui::frame |
                      ftxui::flex);

    auto descriptionBox =
        tasks.empty() ? ftxui::text("No task to show")
                      : ftxui::hbox({ftxui::paragraphAlignLeft(
                            tasks[selected]->getExtendedDescription())});

    // Progress bar or "No task selected" message
    auto progress_section =
        tasks.empty()
            ? ftxui::text("No task selected")
            : ftxui::hbox({
                  ftxui::gaugeRight(tasks[selected]->getProgress() * 0.01) |
                      ftxui::color(ftxui::Color::Green),
                  ftxui::separatorEmpty(),
                  ftxui::text(std::to_string(tasks[selected]->getProgress()) +
                              "\%") |
                      ftxui::bold,
              });

    return ftxui::vbox({
        ftxui::hbox({
            ftxui::vbox({left_menu | ftxui::yflex}) | ftxui::xflex,
            ftxui::vbox({right_menu | ftxui::yflex,
                         ftxui::window(ftxui::text("Progress") | ftxui::bold,
                                       progress_section)}) |
                ftxui::xflex,
        }),
        ftxui::vbox({ftxui::window(ftxui::text("Description") | ftxui::bold,
                                   descriptionBox) |
                     ftxui::flex_grow}) |
            ftxui::yflex,
    });
  });
  return renderer;
}

//----------------------------------------

void MainDialog::handleEvents(ftxui::Component component,
                              ftxui::ScreenInteractive &screen) {
  auto event_handler = ftxui::CatchEvent(component, [&](ftxui::Event event) {
    std::string eventChar = event.character();
    if (eventChar == "n") {
      UserPrompts::taskDialog(screen, false);
      refreshTasksAndMenu();
      selected = tasks.size() - 1;
      return true;
    } else if (eventChar == "q") {
      screen.Exit();
      return true;
    } else if ((eventChar == " " || eventChar == "\n") && !tasks.empty()) {
      Facade::getInstance()->toggleTaskCompleted(tasks[selected]);
      states[selected] = tasks[selected]->isCompleted();
      getMenuEntries();
      return true;
    } else if (eventChar == "r" && !tasks.empty()) {
      auto secondScreen = ftxui::ScreenInteractive::Fullscreen();
      if (UserPrompts::confirmDialog(secondScreen, "Remove task?")) {
        Facade::getInstance()->removeTask(tasks[selected]);
        selected = tasks.size() - 1;
        selectedTag = 0;
        refreshTasksAndMenu();
        return true;
      }
    } else if (eventChar == "+" && !tasks.empty()) {
      int newProgress = tasks[selected]->getProgress() + 10;
      Facade::getInstance()->setTaskProgress(tasks[selected], newProgress);
      tasks[selected]->setProgress(newProgress);
      getMenuEntries();
      return true;
    } else if (eventChar == "-" && !tasks.empty()) {
      int newProgress = tasks[selected]->getProgress() - 10;
      Facade::getInstance()->setTaskProgress(tasks[selected], newProgress);
      tasks[selected]->setProgress(newProgress);
      getMenuEntries();
      return true;
    } else if (eventChar == "\t") {
      tags.size() == 0 ? selectedTag = 0 : selectedTag = (selectedTag + 1) % tags.size();
      refreshTasksAndMenu();
    } else if (eventChar == "e" && !tasks.empty()) {
      UserPrompts::taskDialog(screen, true, tasks[selected]);
      refreshTasksAndMenu();
    }
    return false;
  });
  screen.Loop(event_handler);
}

//----------------------------------------

void MainDialog::refreshTasksAndMenu() {
  tasks = Facade::getInstance()->getTasks();
  updateTasksByTag();
  selected = std::min(selected, static_cast<int>(tasks.size()) - 1);
}

//----------------------------------------

ftxui::Color MainDialog::getEntryColor(int taskIndex) {
  if (tasks[taskIndex]->isCompleted()) {
    return ftxui::Color::GrayDark;
  }
  switch (tasks[taskIndex]->getPriority()) {
  case Priority::Level::High:
    return ftxui::Color::Red;
  case Priority::Level::Medium:
    return ftxui::Color::Orange1;
  default:
    return ftxui::Color::Wheat1;
  }
}
