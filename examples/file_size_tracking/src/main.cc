#include "config.h"
#include "logger.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

#define MAX_THREADS 10
#define MAX_LOG_SIZE 1000000
static int logFileCounter = 0;

namespace fs = std::filesystem;

void maxFileSizeCallback(void *userdata) {
  fs::path *log_dir = (fs::path *) userdata;
  logFileCounter++;
  fs::path logFile = *log_dir / fs::path("log." + std::to_string(logFileCounter) + ".txt");

  log_library_set_log_file_unlocked(std::string(logFile.u8string()).c_str());
}

int main() {
  fs::path cmake_log_dir(fs::absolute(LOG_DIR));
  fs::path log_dir = cmake_log_dir / fs::path("log");
  if (!fs::exists(log_dir)) {
    fs::create_directory(log_dir);
  }

  fs::path logFile = log_dir / fs::path("log." + std::to_string(logFileCounter) + ".txt");

  log_library_set_log_file(std::string(logFile.u8string()).c_str());
  log_library_set_log_max_size(MAX_LOG_SIZE);
  log_library_set_max_file_size_callback(maxFileSizeCallback, (void *) &log_dir);

  auto start = std::chrono::high_resolution_clock::now();
  std::vector<std::thread> threads;
  for (int i = 0; i < MAX_THREADS; i++) {
    threads.push_back(std::thread([&]() {
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