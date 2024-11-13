#pragma once

#include <thread>
#include <vector>


std::vector<std::thread> flushingLogs(int maxThreads);