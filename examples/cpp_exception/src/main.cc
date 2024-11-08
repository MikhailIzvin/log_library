#include "logger.h"

int test(int) {
  throw std::runtime_error(EXCEPTION("An error occurred"));
}

int main() {
  try {
    test(0);
  } catch (const std::exception &e) {
    LOGERROR("Caught exception: %s", e.what());
  }
  return 0;
}