#include "../include/Facade.h"
#include "../include/ScreenElements.h"
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
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
    if (!task->isCompleted()) {
      tasker->updateTaskCompleted(task, true);
    } else {
      tasker->updateTaskCompleted(task, false);
    }
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
                     std::string due) {
  try {
    auto tp = getTimeFromString(due);
    tasker->createTask(description, level, tp);
  } catch (const std::exception &e) {
    std::cerr << "Error while creating new task: " << e.what() << std::endl;
  }
}

std::vector<std::shared_ptr<Task>> Facade::getTasks() {
  try {
    return tasker->getTasks();
  } catch (const std::exception &e) {
    std::cerr << "Error while retrieving tasks: " << e.what() << std::endl;
    // return empty vector or rethrow exception
    return {};
  }
}

void Facade::overwriteTasker(std::string path) {
  this->tasker = std::make_unique<TaskManager>(TaskManager(path));
}

void Facade::setTaskProgress(std::shared_ptr<Task> task, int progress) {
  try {
    this->tasker->updateTaskProgress(task, progress);
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

// TUI Methods
// -----------------

void Facade::tuiCreateTask() {
  try {
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    ScreenElements::newTaskDialog(screen);
  } catch (const std::exception &e) {
    std::cerr << "Error while creating task: " << e.what() << std::endl;
  }
}

void Facade::tuiViewTasks() {
  try {
    ScreenElements::viewTasks();
  } catch (const std::exception &e) {
    std::cerr << "Error in TUI main menu: " << e.what() << std::endl;
    // Handle or rethrow
  }
}
