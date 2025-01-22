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
// void ScreenElements::newTaskDialog(ftxui::ScreenInteractive &screen) {
//   using namespace ftxui;
//
//   std::string inputText;
//   std::string descriptionInputText;
//   std::string dueDateText;
//   std::string tagInputText;
//   auto descriptionInputBox = Input(&inputText, "Title");
//   auto extendedDescriptionInputBox =
//       Input(&descriptionInputText, "description");
//   auto dueDateInputBox = Input(&dueDateText, "DD-MM-YYYY");
//   auto tagInputBox = Input(&tagInputText, "work");
//
//   std::vector<std::string> entries = {"High", "Medium", "Low"};
//   int selected = 0;
//
//   auto style = ButtonOption::Ascii();
//   auto saveBtn = Button(
//       "Save",
//       [&] {
//         if (!inputText.empty() && !dueDateText.empty() &&
//             !tagInputText.empty()) {
//           Facade::getInstance()->newTask(
//               inputText, Priority::fromString(entries[selected]),
//               dueDateText, tagInputText, descriptionInputText);
//           screen.Exit();
//         }
//       },
//       style);
//   auto cancelBtn = Button("Cancel", [&] { screen.Exit(); }, style);
//
//   auto radiobox = Radiobox(&entries, &selected);
//   auto main_container = Container::Vertical({
//       descriptionInputBox,
//       extendedDescriptionInputBox,
//       saveBtn,
//       cancelBtn,
//       radiobox,
//       dueDateInputBox,
//       tagInputBox,
//   });
//
//   auto component = Renderer(main_container, [&] {
//     auto descriptionWindow =
//         window(text("New Task") | bold | center,
//                vbox({descriptionInputBox->Render() | frame |
//                          size(ftxui::HEIGHT, ftxui::EQUAL, 3),
//                      separator(),
//                      extendedDescriptionInputBox->Render() | frame |
//                      flex_grow, separator(), hbox({
//                          saveBtn->Render() | frame | color(Color::Green),
//                          cancelBtn->Render() | frame | color(Color::Red),
//                      }) | size(HEIGHT, ftxui::EQUAL, 1)}));
//
//     auto priorityWindow =
//         window(text("Priority") | bold | center,
//                vbox({radiobox->Render() | vscroll_indicator | frame}));
//
//     auto dueDateWindow = window(text("Due Date") | bold | center,
//                                 vbox({dueDateInputBox->Render() | frame}));
//     auto tagInputWindow = window(text("Tag") | bold | center,
//                                  vbox({tagInputBox->Render() | frame}));
//
//     return vbox({
//         hbox({
//             priorityWindow | xflex,
//             dueDateWindow | xflex,
//             tagInputWindow | xflex,
//
//         }),
//         descriptionWindow | flex_grow,
//     });
//   });
//
//   screen.Loop(component);
// }

void ScreenElements::viewTasks() {
  using namespace ftxui;

  std::vector<std::string> tags = Facade::getInstance()->getAllTags();
  auto tasks = Facade::getInstance()->getTasks();
  int selectedTag = 0;
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
    if (tasks.empty()) {
      menu->Add(MenuEntry("Press 'n' to create a new task."));
    } else {
      for (size_t i = 0; i < tasks.size(); ++i) {
        states[i] = tasks[i]->isCompleted();
        menu->Add(
            Checkbox(format("{}", tasks[i]->getDescription()), &states[i]) |
            color(getEntryColor(i)));
      }
    }
  };

  auto updateTasksByTag = [&]() {
    tags = Facade::getInstance()->getAllTags();
    tasks = Facade::getInstance()->getTasksByTag(tags[selectedTag]);
    getMenuEntries();
  };

  updateTasksByTag();

  auto renderer = Renderer(menu, [&] {
    // Determine the current tag to display
    std::string currentTagText = "none";
    if (!tags.empty()) {
      currentTagText = tags[selectedTag];
    }

    // list of tasks inside a window
    auto left_menu =
        window(text("Tasks") | bold | center,
               vbox({
                   text("Current tag: " + currentTagText) | bold | frame,
                   separator(),
                   menu->Render() | vscroll_indicator | frame | flex,
               })); // details of the selected task (if tasks exist)
    auto right_menu =
        // if no tasks exist --> print 'no task' | else: print details
        tasks.empty()
            ? window(text("Task Details") | bold | center,
                     vbox({text("No tasks to display") | bold}))
            : window(
                  text("Task Details") | bold | center,
                  vbox({
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
                  }) | frame |
                      flex); // combine left and right menus

    auto descriptionBox =
        tasks.empty() ? text("No task to show")
                      : hbox({paragraphAlignLeft(
                            tasks[selected]->getExtendedDescription())});
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
            vbox({
                left_menu | yflex,
            }) | xflex,
            vbox({right_menu | yflex,
                  window(text("Progress") | bold, progress_section)}) |
                xflex,
        }),
        vbox({window(text("Description") | bold, descriptionBox) | flex_grow})

            | yflex,
        // window(text("Navigation") | bold | center,
        //        vbox({
        //            text("space: toggle completed, r: remove, n: new task, q:
        //            "
        //                 "quit") |
        //                center,
        //            text("+/- increase/decrease progress") | center,
        //        })),
    });
  });

  auto screen = ScreenInteractive::Fullscreen();

  auto refreshTasksAndMenu = [&]() {
    tasks = Facade::getInstance()->getTasks(); // refresh tasks
    updateTasksByTag();
    selected = std::min(selected, static_cast<int>(tasks.size()) - 1);
  };

  auto component = CatchEvent(renderer, [&](Event event) {
    std::string eventChar = event.character();

    // new task
    if (eventChar == "n") {
      taskDialog(screen, false);
      refreshTasksAndMenu();
      selected = tasks.size() - 1; // Select the newly created task
      return true;                 // refresh screen
      // quit program
    } else if (eventChar == "q") {
      screen.Exit();
      return true;
      // mark as completed
    } else if (eventChar == " " && !tasks.empty() ||
               eventChar == "\n" && !tasks.empty()) {
      Facade::getInstance()->toggleTaskCompleted(tasks[selected]);
      states[selected] = tasks[selected]->isCompleted();
      getMenuEntries();
      return true;
      // remove task
    } else if (eventChar == "r" && !tasks.empty()) {
      auto secondScreen = ScreenInteractive::Fullscreen();
      if (confirmDialog(secondScreen, "Remove task?")) {
        Facade::getInstance()->removeTask(tasks[selected]);
        selected = tasks.size() - 1; // Select the newly created task
        selectedTag = 0;
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
    } else if (eventChar == "\t") {
      selectedTag = (selectedTag + 1) % tags.size(); // Cycle through tags
      refreshTasksAndMenu();
    } else if (eventChar == "e" && !tasks.empty()) {
      // editTaskDialog(screen, tasks[selected]);
      taskDialog(screen, true, tasks[selected]);
      refreshTasksAndMenu();
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

// void ScreenElements::editTaskDialog(ftxui::ScreenInteractive &screen,
//                                     std::shared_ptr<Task> task) {
//   using namespace ftxui;
//
//   std::string inputText = task->getDescription();
//   std::string descriptionInputText = task->getExtendedDescription();
//   std::string dueDateText = task->getFullDueDate();
//   std::string tagInputText = task->getTag();
//   auto descriptionInputBox = Input(&inputText, "Title");
//   auto extendedDescriptionInputBox =
//       Input(&descriptionInputText, "Description");
//   auto dueDateInputBox = Input(&dueDateText, "DD-MM-YYYY");
//   auto tagInputBox = Input(&tagInputText, "Tag");
//
//   std::vector<std::string> entries = {"High", "Medium", "Low"};
//   int selected = static_cast<int>(task->getPriority());
//
//   auto style = ButtonOption::Ascii();
//   auto saveBtn = Button(
//       "Save",
//       [&] {
//         if (!inputText.empty() && !dueDateText.empty() &&
//             !tagInputText.empty()) {
//           task->setDescription(inputText);
//           task->setExtendedDescription(descriptionInputText);
//           task->setDueDate(Facade::getInstance()->getTimeFromString(
//               dueDateText)); // Ensure the date is converted to the correct
//                              // format
//           task->setTag(tagInputText);
//           task->setPriority(Priority::fromString(entries[selected]));
//           Facade::getInstance()->updateJsonFile(); // Save changes
//           screen.Exit();
//         }
//       },
//       style);
//   auto cancelBtn = Button("Cancel", [&] { screen.Exit(); }, style);
//
//   auto radiobox = Radiobox(&entries, &selected);
//   auto main_container = Container::Vertical({
//       descriptionInputBox,
//       extendedDescriptionInputBox,
//       saveBtn,
//       cancelBtn,
//       radiobox,
//       dueDateInputBox,
//       tagInputBox,
//   });
//
//   auto component = Renderer(main_container, [&] {
//     auto descriptionWindow =
//         window(text("Edit Task") | bold | center,
//                vbox({descriptionInputBox->Render() | frame |
//                          size(ftxui::HEIGHT, ftxui::EQUAL, 3),
//                      separator(),
//                      extendedDescriptionInputBox->Render() | frame |
//                      flex_grow, separator(), hbox({
//                          saveBtn->Render() | frame | color(Color::Green),
//                          cancelBtn->Render() | frame | color(Color::Red),
//                      }) | size(HEIGHT, ftxui::EQUAL, 1)}));
//
//     auto priorityWindow =
//         window(text("Priority") | bold | center,
//                vbox({radiobox->Render() | vscroll_indicator | frame}));
//
//     auto dueDateWindow = window(text("Due Date") | bold | center,
//                                 vbox({dueDateInputBox->Render() | frame}));
//     auto tagInputWindow = window(text("Tag") | bold | center,
//                                  vbox({tagInputBox->Render() | frame}));
//
//     return hbox({descriptionWindow | flex, vbox({
//                                                priorityWindow | xflex,
//                                                dueDateWindow | xflex,
//                                                tagInputWindow | yflex,
//                                            })});
//   });
//
//   screen.Loop(component);
// }

void ScreenElements::taskDialog(ftxui::ScreenInteractive &screen, bool isEdit,
                                std::shared_ptr<Task> task) {
  using namespace ftxui;

  std::string inputText = isEdit && task ? task->getDescription() : "";
  std::string descriptionInputText =
      isEdit && task ? task->getExtendedDescription() : "";
  std::string dueDateText = isEdit && task ? task->getFullDueDate() : "";
  std::string tagInputText = isEdit && task ? task->getTag() : "";
  auto descriptionInputBox = Input(&inputText, "Title");
  auto extendedDescriptionInputBox =
      Input(&descriptionInputText, "Description");
  auto dueDateInputBox = Input(&dueDateText, "DD-MM-YYYY");
  auto tagInputBox = Input(&tagInputText, "Tag");

  std::vector<std::string> entries = {"High", "Medium", "Low"};
  int selected =
      isEdit && task
          ? std::distance(entries.begin(),
                          std::find(entries.begin(), entries.end(),
                                    Priority::toString(task->getPriority())))
          : 0;

  auto style = ButtonOption::Ascii();
  auto saveBtn = Button(
      isEdit ? "Save" : "Create",
      [&] {
        if (!inputText.empty() && !dueDateText.empty() &&
            !tagInputText.empty()) {
          if (isEdit && task) {
            // editing an existing task
            task->setDescription(inputText);
            task->setExtendedDescription(descriptionInputText);
            task->setDueDate(Facade::getInstance()->getTimeFromString(
                dueDateText)); // ensure the date is converted to the correct
                               // format
            task->setTag(tagInputText);
            task->setPriority(Priority::fromString(entries[selected]));
          } else {
            // creating a new task
            Facade::getInstance()->newTask(
                inputText, Priority::fromString(entries[selected]), dueDateText,
                tagInputText, descriptionInputText);
          }
          Facade::getInstance()->updateJsonFile(); // save changes to the file
          screen.Exit();
        }
      },
      style);
  auto cancelBtn = Button("Cancel", [&] { screen.Exit(); }, style);

  auto radiobox = Radiobox(&entries, &selected);
  auto main_container = Container::Vertical({
      descriptionInputBox,
      extendedDescriptionInputBox,
      saveBtn,
      cancelBtn,
      radiobox,
      dueDateInputBox,
      tagInputBox,
  });

  auto component = Renderer(main_container, [&] {
    auto descriptionWindow =
        window(text(isEdit ? "Edit Task" : "New Task") | bold | center,
               vbox({descriptionInputBox->Render() | frame |
                         size(ftxui::HEIGHT, ftxui::EQUAL, 3),
                     separator(),
                     extendedDescriptionInputBox->Render() | frame | flex_grow,
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
    auto tagInputWindow = window(text("Tag") | bold | center,
                                 vbox({tagInputBox->Render() | frame}));

    return vbox({
        hbox({
            priorityWindow | xflex,
            dueDateWindow | xflex,
            tagInputWindow | xflex,

        }),
        descriptionWindow | flex_grow,
    });
  });
  screen.Loop(component);
}
