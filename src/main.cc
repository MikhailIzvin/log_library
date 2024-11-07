#include "logger.h"

class Test {
public:
  Test() {
    LOGINFO("Test object created");
  }
  ~Test() {
    LOGINFO("Test object destroyed");
  }
  void call() {
    throw std::runtime_error(EXCEPTION("This is a runtime error"));
  }
};

int main() {
  // log_library_set_log_file("log.txt");
  LOGDEBUG("This is a debug message");
  try {
    Test t;
    t.call();
  } catch (const std::exception &e) {
    LOGERROR("%s", e.what());
  }
}