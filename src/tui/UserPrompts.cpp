#include "../../include/tui/UserPrompts.h"
#include "../../include/Facade.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

//----------------------------------------

bool UserPrompts::confirmDialog(ftxui::ScreenInteractive &screen,
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

  screen.Loop(dialog_renderer);

  return result;
}

//----------------------------------------

void UserPrompts::taskDialog(ftxui::ScreenInteractive &screen, bool isEdit,
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
