#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include <chrono>
#include <memory> // Include the memory header
#include <vector>

class TaskManager {
public:
  TaskManager(std::string filePath);
  std::vector<std::shared_ptr<Task>> getTasks() const;
  void createTask(const std::string &description, Priority::Level level,
                  std::chrono::system_clock::time_point due);
  void saveTasksToJson() const;
  void getTasksFromJson();
  void updateTaskCompleted(std::shared_ptr<Task> task, bool currentState);
  void removeTask(std::shared_ptr<Task> task);
  void setFilePath(std::string path);
  void updateTaskProgress(std::shared_ptr<Task> task, int progress);
  std::shared_ptr<Task> getTaskById(int id) const;
  void cleanJsonFile();
  void updateDueDate(std::shared_ptr<Task> &task,
                     std::chrono::system_clock::time_point due);

private:
  std::vector<std::shared_ptr<Task>> tasks;
  std::string filePath;
  int calculateId() const;
  void autoJsonCreate();
};

#endif // TASKMANAGER_H
