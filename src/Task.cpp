#include "../include/Task.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <nlohmann/json_fwd.hpp>
#include <sstream>
#include <string>

// GETTERS & SETTERS

std::string Task::getDescription() const { return this->description; }
void Task::setDescription(std::string desc) { this->description = desc; }

std::string Task::getExtendedDescription() const {
  return this->extendedDescription;
}

void Task::setExtendedDescription(std::string longText) {
  this->extendedDescription = longText;
}

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
void Task::setPriority(Priority::Level prior) { this->priority = prior; }

int Task::getProgress() const { return this->progress; }
void Task::setProgress(int num) {
  this->progress = std::min(num, 100);

  if (progress == 100) {
    this->completed = true;
  } else {
    this->completed = false;
  }
}

void Task::setTag(std::string tagName) { this->tag = tagName; }
std::string Task::getTag() const { return this->tag; }

void Task::setDueDate(const std::chrono::system_clock::time_point &dueDate) {
  this->dueDate = dueDate;
}

std::string Task::getFullDueDate() const {
  std::time_t due_time = std::chrono::system_clock::to_time_t(dueDate);
  std::tm due_tm = *std::localtime(&due_time);
  std::ostringstream oss;
  oss << std::put_time(&due_tm, "%d-%m-%Y");
  return oss.str();
}

std::chrono::system_clock::time_point Task::getDueDate() {
  return this->dueDate;
}

std::string Task::getRelativeDueDate() const {
  using namespace std::chrono;
  auto now = system_clock::now();

  auto duration = duration_cast<seconds>(dueDate - now);
  auto days = duration.count() / (60 * 60 * 24);

  // Format to string
  if (days == 0) {
    return "Today";
  } else if (days == 1) {
    return "In 1 day";
  } else if (days > 1) {
    return "In " + std::to_string(days) + " days";
  } else {
    return std::to_string(-days) + " days ago";
  };
}

std::string Task::getAbsoluteTimeMessage() const {
  std::time_t creation_time =
      std::chrono::system_clock::to_time_t(creationTime);
  std::tm creation_tm = *std::localtime(&creation_time);
  std::ostringstream oss;
  oss << std::put_time(&creation_tm, "%d-%m-%Y");
  return oss.str();
}

// CONSTRUCTOR

Task::Task(std::string desc, Priority::Level priority,
           std::chrono::system_clock::time_point dueDate,
           const std::string &tagg, const std::string &longtext)
    : tag(tagg), extendedDescription(longtext) {
  this->completed = false;
  this->description = desc;
  this->priority = priority;
  this->id = 0;
  this->creationTime = std::chrono::system_clock::now();
  this->dueDate = dueDate; // Initialize dueDate to creationTime or
                           // some default value
  this->progress = 0;
}

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
  auto creationTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                                   creationTime.time_since_epoch())
                                   .count();
  auto dueTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                              dueDate.time_since_epoch())
                              .count();

  j["ID"] = this->id;
  j["description"] = this->description;
  j["extended"] = this->extendedDescription;
  j["priority"] = Priority::toString(this->priority);
  j["creationTime"] = creationTimeInSeconds;
  j["dueDate"] = dueTimeInSeconds;
  j["completed"] = this->completed;
  j["tag"] = this->tag;
  j["progress"] = this->progress;

  return j;
}

Task Task::from_json(const nlohmann::json &file) {
  std::string description = file["description"];
  std::string extendedDesc = file["extended"];
  std::string priority = file["priority"];
  bool completed = file["completed"];
  int id = file["ID"];
  auto creationTimeInSeconds = file["creationTime"].get<long long>();
  auto dueTimeInSeconds = file["dueDate"].get<long long>();
  int progress = file["progress"];
  std::string tag = file["tag"];

  std::chrono::system_clock::time_point creationTime =
      std::chrono::system_clock::from_time_t(creationTimeInSeconds);
  std::chrono::system_clock::time_point dueDate =
      std::chrono::system_clock::from_time_t(dueTimeInSeconds);

  Task task(description, Priority::fromString(priority), dueDate);
  task.extendedDescription = extendedDesc;
  task.creationTime = creationTime;
  task.completed = completed;
  task.id = id;
  task.progress = progress;
  task.tag = tag;
  return task;
}
