#ifndef PRIORITY
#define PRIORITY

#include <string>

class Priority {
public:
  enum class Level { Low, Medium, High };

  static std::string toString(Level level);
  static Priority::Level fromString(std::string level);
};
#endif // !PRIORITY
