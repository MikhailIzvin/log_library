#include "logger.h"

void init() {
  throw_runtime_error("init failed");
}

int main() {

  try {
    init();
  } catch (const std::exception &e) {
    LOGERROREXCEPTION(e.what());
  }
}