#include "../include/Facade.h"
#include "../include/ScreenElements.h"
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Apply singleton so there's only one instance of Facade
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

void Facade::toggleTaskCompleted(Task &task) {
  try {
    if (!task.isCompleted()) {
      tasker->updateTaskCompleted(task, true);
    } else {
      tasker->updateTaskCompleted(task, false);
    }
  } catch (const std::exception &e) {
    std::cerr << "Error while modifying task state: " << e.what() << std::endl;
  }
}

void Facade::removeTask(Task &task) {
  try {
    tasker->removeTask(task);
  } catch (const std::exception &e) {
    std::cerr << "Error while removing task: " << e.what() << std::endl;
  }
}

void Facade::newTask(const std::string &description, Priority::Level level) {
  try {
    tasker->createTask(description, level);
  } catch (const std::exception &e) {
    std::cerr << "Error while creating new task: " << e.what() << std::endl;
  }
}

std::vector<Task> Facade::getTasks() {
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

void Facade::setTaskProgress(Task &task, int progress) {
  try {
    this->tasker->updateTaskProgress(task, progress);
  } catch (const std::exception &e) {
    std::cerr << "Updating progress failed: " << e.what() << std::endl;
  }
}

std::unique_ptr<TaskManager> Facade::getTasker() {
  return std::move(this->tasker);
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
