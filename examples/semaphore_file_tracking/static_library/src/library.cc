#include "library.hh"
#include "logger.h"


std::vector<std::thread> flushingLogs(int maxThreads) {
  LOGDEBUG("LIBRARY_TAG", "Library test");
  std::vector<std::thread> threads;
  for (int i = 0; i < maxThreads; i++) {
    threads.push_back(std::thread([i]() {
      for (int j = 0; j < 10000; j++) {
        std::thread::id this_id = std::this_thread::get_id();

        LOGDEBUG("LIBRARY_TAG", "Thread %d started with id %d", i, this_id);
        LOGINFO("LIBRARY_TAG", "Thread %d started with id %d", i, this_id);
        LOGERROR("LIBRARY_TAG", "Thread %d started with id %d", i, this_id);
        LOGWARN("LIBRARY_TAG", "Thread %d started with id %d", i, this_id);
      }
    }));
  }
  return threads;
}