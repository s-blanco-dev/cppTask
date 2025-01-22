#ifndef PRIORITY
#define PRIORITY

#include <string>

class Priority {
public:
  enum class Level { High, Medium, Low };

  static std::string toString(Level level);
  static Priority::Level fromString(std::string level);
};
#endif // !PRIORITY
