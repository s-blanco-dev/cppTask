#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include <memory> // Include the memory header
#include <vector>

class TaskManager {
public:
  TaskManager(std::string filePath);
  std::vector<std::shared_ptr<Task>> getTasks() const;
  void createTask(const std::string &description, Priority::Level level);
  void saveTasksToJson(const std::string &filename) const;
  void getTasksFromJson(const std::string &filename);
  void updateTaskCompleted(std::shared_ptr<Task> task, bool currentState);
  void removeTask(std::shared_ptr<Task> task);
  void setFilePath(std::string path);
  void updateTaskProgress(std::shared_ptr<Task> task, int progress);
  std::shared_ptr<Task> getTaskById(int id) const;
  void cleanJsonFile();

private:
  std::vector<std::shared_ptr<Task>> tasks;
  std::string filePath;
  int calculateId() const;
  void autoJsonCreate(const std::string &filename);
};

#endif // TASKMANAGER_H
