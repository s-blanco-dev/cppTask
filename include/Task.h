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
  std::string extendedDescription;
  bool completed;
  Priority::Level priority;
  std::chrono::system_clock::time_point creationTime;
  std::chrono::system_clock::time_point dueDate;
  int id;
  int progress;
  std::string tag;

public:
  virtual ~Task() =
      default; // virtual destructor for proper cleanup in derived classes
  std::string getDescription() const;
  void setDescription(std::string description);

  std::string getExtendedDescription() const;
  void setExtendedDescription(std::string longText);

  bool isCompleted() const;
  void setCompleted(bool state);

  int getId() const;
  void setId(int number);

  Priority::Level getPriority() const;

  int getProgress() const;
  void setProgress(int prog);

  void setTag(std::string tagName);
  std::string getTag() const;

  // std::chrono::system_clock::time_point getAbsoluteCreationTime() const;

  void setDueDate(const std::chrono::system_clock::time_point &due);
  std::string getRelativeTimeMessage() const;
  std::string getAbsoluteTimeMessage() const;
  std::string getRelativeDueDate() const;
  std::string getFullDueDate() const;

  nlohmann::json to_json() const;
  static Task from_json(const nlohmann::json &file);

  Task(std::string description, Priority::Level priority,
       std::chrono::system_clock::time_point dueDate,
       const std::string &tag = "default",
       const std::string &extendedDescription = "");
};

#endif // !TASK
