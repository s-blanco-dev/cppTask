#ifndef FACADE
#define FACADE

#include "Task.h"
#include "TaskManager.h"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
class Facade {
private:
  static Facade *instance;
  Facade();

  std::unique_ptr<TaskManager> tasker;

public:
  static Facade *getInstance(); // singleton instance
  ~Facade();                    // destructor
  static void resetInstance();

  void tuiCreateTask();
  void tuiViewTasks();
  void toggleTaskCompleted(std::shared_ptr<Task> task);
  void removeTask(std::shared_ptr<Task> task);
  void newTask(const std::string &description, Priority::Level level,
               std::string due, const std::string &tag = "default",
               const std::string &extended = "");
  std::vector<std::shared_ptr<Task>> getTasks();
  void overwriteTasker(std::string path);
  void setTaskProgress(std::shared_ptr<Task> task, int progress);
  std::chrono::system_clock::time_point
  getTimeFromString(const std::string &time);
  std::unique_ptr<TaskManager> getTasker();
  void cleanTaskerJsonFile();
  std::vector<std::shared_ptr<Task>> getTasksByTag(const std::string &tagName);
  std::vector<std::string> getAllTags() const;
  void updateJsonFile();
};

#endif // !FACADE
