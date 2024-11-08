#include "logger.h"


int main() {
  LOGDEBUG("TAG1", "This is a debug message");
  LOGINFO("TAG2", "This is an info message");
  LOGWARN("TAG3", "This is a warning message");
  LOGERROR("TAG4", "This is an error message");
}