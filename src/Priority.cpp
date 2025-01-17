// Priority.cpp
#include "../include/Priority.h"

std::string Priority::toString(Level level) {
  switch (level) {
  case Level::Low:
    return "Low";
  case Level::Medium:
    return "Medium";
  case Level::High:
    return "High";
  default:
    return "Unknown";
  }
}

Priority::Level Priority::fromString(std::string level) {
  if (level.compare("Low") == 0) {
    return Level::Low;
  } else if (level.compare("Medium") == 0) {
    return Level::Medium;
  } else if (level.compare("High") == 0) {
    return Level::High;
  } else {
    return Level::Medium;
  }
}
