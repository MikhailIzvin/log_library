#include "logger.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>

#define MAX_THREADS 10

int main() {
  log_library_set_log_file("log.txt");
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<std::thread> threads;
  for (int i = 0; i < MAX_THREADS; i++) {
    threads.push_back(std::thread([i]() {
      for (int j = 0; j < 10000; j++) {
        std::thread::id this_id = std::this_thread::get_id();

        LOGDEBUG("Thread %d started with id %d", i, this_id);
        LOGINFO("Thread %d started with id %d", i, this_id);
        LOGERROR("Thread %d started with id %d", i, this_id);
        LOGWARN("Thread %d started with id %d", i, this_id);
      }
    }));
  }

  for (auto &thread: threads) {
    thread.join();
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << "s\n";
}