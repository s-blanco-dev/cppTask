#ifndef TASKMANAGER
#define TASKMANAGER

#include "Task.h"
#include <string>
#include <vector>
class TaskManager {
private:
  std::vector<Task> tasks;
  std::string filePath;
  int calculateId() const;
  void saveTasksToJson(const std::string &filename) const;
  void getTasksFromJson(const std::string &filename);
  void autoJsonCreate(const std::string &filename);

public:
  std::vector<Task> getTasks() const;
  // void addTask(Task &task);
  void createTask(const std::string &description, Priority::Level level);
  void updateTaskCompleted(Task &task, bool currentSate);
  void removeTask(Task &task);
  void setFilePath(std::string filePath);
  void updateTaskProgress(Task &task, int progress);
  Task getTaskById(int id) const;

  TaskManager(std::string filePath);
};

#endif // !TASKMANAGER
