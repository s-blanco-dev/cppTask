#ifndef FACADE
#define FACADE

#include "Task.h"
#include "TaskManager.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
class Facade {
private:
  static Facade *instance;
  Facade();

  std::unique_ptr<TaskManager> tasker;

public:
  static Facade *getInstance(); // instancia única de Singleton
  ~Facade();                    // Destructor
  static void resetInstance();

  void tuiCreateTask();
  void tuiViewTasks();
  void toggleTaskCompleted(Task &task);
  void removeTask(Task &task);
  void newTask(const std::string &description, Priority::Level level);
  std::vector<Task> getTasks();
  void overwriteTasker(std::string path);
  void setTaskProgress(Task &task, int progress);
  std::unique_ptr<TaskManager> getTasker();
};

#endif // !FACADE
