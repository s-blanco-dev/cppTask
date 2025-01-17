#include "../include/Task.h"
#include <algorithm>
#include <nlohmann/json_fwd.hpp>
#include <string>

// GETTERS & SETTERS

std::string Task::getDescription() const { return this->description; }
void Task::setDescription(std::string desc) { this->description = desc; }

bool Task::isCompleted() const { return this->completed; }
void Task::setCompleted(bool state) {
  if (state) {
    this->setProgress(100);
  } else {
    this->setProgress(0);
  }

  this->completed = state;
}

int Task::getId() const { return this->id; }
void Task::setId(int number) { this->id = number; }

Priority::Level Task::getPriority() const { return this->priority; }

int Task::getProgress() const { return this->progress; }
void Task::setProgress(int num) {
  this->progress = std::min(num, 100);

  if (progress == 100) {
    this->completed = true;
  } else {
    this->completed = false;
  }
}

// CONSTRUCTOR

Task::Task(std::string desc, Priority::Level priority) {
  this->completed = false;
  this->description = desc;
  this->priority = priority;
  this->id = 0;
  this->creationTime = std::chrono::system_clock::now();
  this->progress = 0;
}

// std::string Task::getRelativeTimeMessage() const { return ""; }
std::string Task::getRelativeTimeMessage() const {

  using namespace std::chrono;
  auto now = system_clock::now();

  auto duration = duration_cast<seconds>(now - creationTime);
  auto days = duration.count() / (60 * 60 * 24);

  // Format to string
  if (days == 0) {
    return "Today";
  } else if (days == 1) {
    return "1 day ago";
  } else {
    return std::to_string(days) + " days ago";
  };
}

nlohmann::json Task::to_json() const {
  nlohmann::json j;
  auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                           creationTime.time_since_epoch())
                           .count();

  j["ID"] = this->id;
  j["description"] = this->description;
  j["priority"] = Priority::toString(this->priority);
  j["creationTime"] = timeInSeconds;
  j["completed"] = this->completed;
  j["progress"] = this->progress;

  return j;
}

Task Task::from_json(const nlohmann::json &file) {
  std::string description = file["description"];
  std::string priority = file["priority"];
  bool completed = file["completed"];
  int id = file["ID"];
  auto timeInSeconds = file["creationTime"].get<long long>();
  int progress = file["progress"];

  // Convertir los segundos de vuelta a un time_point
  std::chrono::system_clock::time_point creationTime =
      std::chrono::system_clock::from_time_t(timeInSeconds);

  Task task(description, Priority::fromString(priority));
  task.creationTime = creationTime;
  task.completed = completed;
  task.id = id;
  task.progress = progress;
  return task;
}
