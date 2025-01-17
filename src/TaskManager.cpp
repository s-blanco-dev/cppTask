#include "../include/TaskManager.h"
#include "../include/Task.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <vector>

TaskManager::TaskManager(std::string filePath) {
  this->filePath = filePath;
  autoJsonCreate(filePath);
  getTasksFromJson(filePath);
}

std::vector<std::shared_ptr<Task>> TaskManager::getTasks() const {
  return tasks;
}

void TaskManager::createTask(const std::string &description,
                             Priority::Level level) {
  auto tasky = std::make_shared<Task>(description, level);
  tasky->setId(calculateId());
  this->tasks.push_back(tasky);
  saveTasksToJson(this->filePath);
}

void TaskManager::saveTasksToJson(const std::string &filename) const {
  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + filename);
  }

  nlohmann::json j;
  for (const auto &task : tasks) {
    j.push_back(task->to_json());
  }

  outFile << j.dump(4); // write to json file
  if (outFile.fail()) {
    throw std::ios_base::failure("Failed to write to file: " + filename);
  }
  outFile.close();
}

void TaskManager::getTasksFromJson(const std::string &filename) {
  std::ifstream inFile(filename);
  if (!inFile.is_open()) {
    throw std::ios_base::failure("Failed to open file: " + filename);
  }

  nlohmann::json j;
  inFile >> j; // read json from file
  if (inFile.fail()) {
    throw std::ios_base::failure("Error reading JSON data from file: " +
                                 filename);
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
  bool taskFound = false;
  for (auto &tasky : this->tasks) {
    if (tasky->getId() == task->getId()) {
      tasky->setCompleted(currentState);
      saveTasksToJson(this->filePath);
      taskFound = true;
      break;
    }
  }
  if (!taskFound) {
    throw std::invalid_argument("Task with ID " +
                                std::to_string(task->getId()) + " not found.");
  }
}

void TaskManager::removeTask(std::shared_ptr<Task> task) {
  bool taskFound = false;

  // iterate through the tasks to find task by id
  for (auto it = tasks.begin(); it != tasks.end(); ++it) {
    if ((*it)->getId() == task->getId()) {
      tasks.erase(it); // remove task according to its id
      saveTasksToJson(this->filePath);
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
  for (auto &tasky : this->tasks) {
    if (tasky->getId() == task->getId()) {
      tasky->setProgress(progress);
      saveTasksToJson(this->filePath);
      break;
    }
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

void TaskManager::autoJsonCreate(const std::string &filename) {
  std::ifstream inFile(filename);

  // If the file does not exist, create it and write 'null' inside
  if (!inFile) {
    // Create an empty JSON object with a 'null' value
    nlohmann::json jsonData = nullptr;

    std::ofstream outFile(filename);
    if (outFile.is_open()) {
      outFile << jsonData.dump(4); // Pretty print with indentations
      outFile.close();
      std::cout << "File created: " << filename << " with 'null' inside."
                << std::endl;
    } else {
      std::cerr << "Failed to create file: " << filename << std::endl;
    }
  } else {
    std::cout << "File already exists: " << filename << std::endl;
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
