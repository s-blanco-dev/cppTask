#ifndef USERPROMPTS
#define USERPROMPTS

#include "../Task.h"
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <string>
class UserPrompts {
public:
  static bool confirmDialog(ftxui::ScreenInteractive &screen,
                            const std::string &message);
  static void taskDialog(ftxui::ScreenInteractive &screen, bool isEdit,
                         std::shared_ptr<Task> task = nullptr);
};

#endif // !USERPROMPTS
