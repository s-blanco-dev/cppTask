#include "../include/TaskManager.h"
#include "../include/Task.h"
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

TaskManager::TaskManager(std::string filePath) {
  this->filePath = filePath;
  autoJsonCreate();
  getTasksFromJson();
}

std::vector<std::shared_ptr<Task>> TaskManager::getTasks() const {
  return tasks;
}

void TaskManager::createTask(const std::string &description,
                             Priority::Level level,
                             std::chrono::system_clock::time_point due,
                             const std::string &tag) {
  auto tasky = std::make_shared<Task>(description, level, due, tag);
  tasky->setId(calculateId());
  this->tasks.push_back(tasky);
  saveTasksToJson();
}

void TaskManager::saveTasksToJson() const {
  std::ofstream outFile(this->filePath);
  if (!outFile.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + this->filePath);
  }

  nlohmann::json j;
  for (const auto &task : tasks) {
    j.push_back(task->to_json());
  }

  outFile << j.dump(4); // write to json file
  if (outFile.fail()) {
    throw std::ios_base::failure("Failed to write to file: " + this->filePath);
  }
  outFile.close();
}

void TaskManager::getTasksFromJson() {
  std::ifstream inFile(this->filePath);
  if (!inFile.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + this->filePath);
  }

  nlohmann::json j;
  inFile >> j; // read json from file
  if (inFile.fail()) {
    throw std::ios_base::failure("Error reading JSON data from file: " +
                                 this->filePath);
  }

  for (const auto &taskJson : j) {
    auto task = std::make_shared<Task>(
        Task::from_json(taskJson)); // convert json to Task object
    this->tasks.push_back(task);    // add task to list
  }

  inFile.close();
}

void TaskManager::updateTaskCompleted(std::shared_ptr<Task> task,
                                      bool currentState) {
  try {
    task->setCompleted(currentState);
    saveTasksToJson();
  } catch (const std::exception &e) {
    std::cerr << "Error updating task completion: " << e.what();
  }
}

void TaskManager::removeTask(std::shared_ptr<Task> task) {
  bool taskFound = false;

  for (auto it = tasks.begin(); it != tasks.end(); ++it) {
    if ((*it)->getId() == task->getId()) {
      tasks.erase(it); // remove task according to its id
      saveTasksToJson();
      taskFound = true;
      break;
    }
  }

  if (!taskFound) {
    throw std::invalid_argument("Task with ID " +
                                std::to_string(task->getId()) + " not found.");
  }
}

void TaskManager::setFilePath(std::string path) { this->filePath = path; }

void TaskManager::updateTaskProgress(std::shared_ptr<Task> task, int progress) {
  try {
    task->setProgress(progress);
    saveTasksToJson();
  } catch (const std::exception &e) {
    std::cerr << "Error updating task progress: " << e.what();
  }
}

void TaskManager::cleanJsonFile() {
  std::ofstream outFile(filePath);
  if (!outFile.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + filePath);
  }

  nlohmann::json emptyJson = nlohmann::json::array();
  outFile << emptyJson.dump(4); // write an empty json array to the file

  if (outFile.fail()) {
    throw std::ios_base::failure("Failed to write to file: " + filePath);
  }
  outFile.close();

  tasks.clear();
}

void TaskManager::updateDueDate(std::shared_ptr<Task> &task,
                                std::chrono::system_clock::time_point due) {
  task->setDueDate(due);
}

// PRIVATE METHODS

int TaskManager::calculateId() const {
  if (this->tasks.empty()) {
    return 1;
  }

  int highestId = 0;
  for (const auto &task : this->tasks) {
    if (task->getId() > highestId) {
      highestId = task->getId();
    }
  }
  return highestId + 1;
}

void TaskManager::autoJsonCreate() {
  std::ifstream inFile(this->filePath);

  // If the file does not exist, create it and write 'null' inside
  if (!inFile) {
    // Create an empty JSON object with a 'null' value
    nlohmann::json jsonData = nullptr;

    std::ofstream outFile(this->filePath);
    if (outFile.is_open()) {
      outFile << jsonData.dump(4); // Pretty print with indentations
      outFile.close();
      std::cout << "File created: " << this->filePath << " with 'null' inside."
                << std::endl;
    } else {
      std::cerr << "Failed to create file: " << this->filePath << std::endl;
    }
  } else {
    std::cout << "File already exists: " << this->filePath << std::endl;
  }
}

std::shared_ptr<Task> TaskManager::getTaskById(int id) const {
  for (const auto &task : this->tasks) {
    if (task->getId() == id) {
      return task;
    }
  }
  throw std::invalid_argument("Task not found");
}
