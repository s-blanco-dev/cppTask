#include "../include/Facade.h"
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// apply singleton so there's only one instance of Facade
Facade *Facade::instance = nullptr;

Facade *Facade::getInstance() {
  if (instance == nullptr) {
    instance = new Facade();
  }
  return instance;
}

// Generic methods
// -----------------
Facade::Facade() {
  try {
    this->tasker = nullptr;
  } catch (const std::bad_alloc &e) {
    std::cerr << "Memory allocation failed while creating TaskManager: "
              << e.what() << std::endl;
    throw; // rethrow exception
  }
}

Facade::~Facade() {}

void Facade::resetInstance() {
  delete instance;
  instance = nullptr;
}

void Facade::toggleTaskCompleted(std::shared_ptr<Task> task) {
  try {
    task->isCompleted() ? task->setCompleted(false) : task->setCompleted(true);
    tasker->saveTasksToJson();
  } catch (const std::exception &e) {
    std::cerr << "Error while modifying task state: " << e.what() << std::endl;
  }
}

void Facade::removeTask(std::shared_ptr<Task> task) {
  try {
    tasker->removeTask(task);
  } catch (const std::exception &e) {
    std::cerr << "Error while removing task: " << e.what() << std::endl;
  }
}

void Facade::newTask(const std::string &description, Priority::Level level,
                     std::string due, const std::string &tag,
                     const std::string &extended) {
  try {
    auto tp = getTimeFromString(due);
    tasker->createTask(description, level, tp, tag, extended);
  } catch (const std::exception &e) {
    std::cerr << "Error while creating new task: " << e.what() << std::endl;
  }
}

std::vector<std::shared_ptr<Task>> Facade::getTasks() {
  try {
    auto tasks = tasker->getTasks();
    return tasks;
  } catch (const std::exception &e) {
    std::cerr << "Error while retrieving tasks: " << e.what() << std::endl;
    // return empty vector or rethrow exception
    return {};
  }
}
void Facade::overwriteTasker(std::string path) {
  try {
    this->tasker = std::make_unique<TaskManager>(TaskManager(path));
  } catch (const std::exception &e) {

    std::cerr << "Error overwriting task manager: " << e.what();
  }
}

void Facade::setTaskProgress(std::shared_ptr<Task> task, int progress) {
  try {
    task->setProgress(progress);
    tasker->saveTasksToJson();
  } catch (const std::exception &e) {
    std::cerr << "Updating progress failed: " << e.what() << std::endl;
  }
}

std::unique_ptr<TaskManager> Facade::getTasker() {
  return std::move(this->tasker);
}

void Facade::cleanTaskerJsonFile() {
  try {
    this->tasker->cleanJsonFile();
  } catch (const std::exception &e) {
    std::cerr << "Error cleaning json file: " << e.what() << std::endl;
  }
}

std::chrono::system_clock::time_point
Facade::getTimeFromString(const std::string &dueDateStr) {
  try {
    // parse the date string
    std::tm tm = {};
    std::istringstream ss(dueDateStr);
    ss >> std::get_time(&tm, "%d-%m-%Y");
    if (ss.fail()) {
      throw std::invalid_argument("Invalid date format, expected dd-mm-yyyy");
    }

    // Convert to time_point
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    return tp;
  } catch (const std::exception &e) {
    std::cerr << "Error while setting due date: " << e.what() << std::endl;
  }
  return {};
}

std::vector<std::shared_ptr<Task>>
Facade::getTasksByTag(const std::string &tagName) {
  std::vector<std::shared_ptr<Task>> tasksWithTag;
  try {
    auto taskerTasks = this->tasker->getTasks();
    for (auto &task : taskerTasks) {
      if (task->getTag() == tagName) {
        tasksWithTag.push_back(task);
      }
    }

    // sort tasks by due date
    std::sort(
        tasksWithTag.begin(), tasksWithTag.end(),
        [](const std::shared_ptr<Task> &a, const std::shared_ptr<Task> &b) {
          return a->getDueDate() < b->getDueDate();
        });

  } catch (const std::exception &e) {
    std::cerr << "Error obtaining tasks by tag: " << e.what();
  }
  return tasksWithTag;
}

std::vector<std::string> Facade::getAllTags() const {
  auto taskerTasks = this->tasker->getTasks();
  std::unordered_set<std::string> uniqueTags;
  for (const auto &task : taskerTasks) {
    if (!task->getTag().empty()) {
      uniqueTags.insert(task->getTag());
    }
  }
  return std::vector<std::string>(uniqueTags.begin(), uniqueTags.end());
}

void Facade::updateJsonFile() {
  try {
    tasker->saveTasksToJson();
  } catch (std::exception &e) {
    std::cerr << "Error updating JSON file: " << e.what();
  }
}
