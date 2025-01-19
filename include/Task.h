#ifndef TASK
#define TASK

#include "Priority.h"
#include "nlohmann/json.hpp"
#include <chrono>
#include <nlohmann/json_fwd.hpp>
#include <string>

class Task {
private:
  std::string description;
  bool completed;
  Priority::Level priority;
  std::chrono::system_clock::time_point creationTime;
  std::chrono::system_clock::time_point dueDate;
  int id;
  int progress;

public:
  virtual ~Task() =
      default; // virtual destructor for proper cleanup in derived classes
  std::string getDescription() const;
  void setDescription(std::string description);
  bool isCompleted() const;
  void setCompleted(bool state);
  void setDueDate(const std::chrono::system_clock::time_point &due);
  int getId() const;
  void setId(int number);
  Priority::Level getPriority() const;
  int getProgress() const;
  void setProgress(int prog);

  // std::chrono::system_clock::time_point getAbsoluteCreationTime() const;
  std::string getRelativeTimeMessage() const;
  std::string getAbsoluteTimeMessage() const;
  std::string getRelativeDueDate() const;
  std::string getFullDueDate() const;
  nlohmann::json to_json() const;
  static Task from_json(const nlohmann::json &file);

  Task(std::string description, Priority::Level priority,
       std::chrono::system_clock::time_point dueDate);
};

#endif // !TASK
