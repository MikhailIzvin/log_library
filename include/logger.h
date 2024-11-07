#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef __cplusplus
#include <stdexcept>
#include <string>
#endif

#define LOG_LIBRFARY_TIME_BUFFER_SIZE 30

// Console colors
#define COLOR_RESET "\033[0m"
#define COLOR_BLUE "\033[34m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"

// Define a mutex for thread safety
#if defined(_WIN32) || defined(_WIN64)
static HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
#define LOCK() WaitForSingleObject(mutex, INFINITE)
#define UNLOCK() ReleaseMutex(mutex)
#define __SHORT_FILE__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK() pthread_mutex_lock(&mutex)
#define UNLOCK() pthread_mutex_unlock(&mutex)
#define __SHORT_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef LOG_LIBRARY_PRETTY_FUNCTION
#define FUNC_NAME __PRETTY_FUNCTION__
#else
#define FUNC_NAME __func__
#endif

// Static log file pointer, defaults to stderr
static FILE *log_library_log_file = NULL;

// Sets the log file. If not set, logs default to stderr.
static inline void log_library_set_log_file(const char *file_path) {
  LOCK();
  if (log_library_log_file && log_library_log_file != stderr) {
    fclose(log_library_log_file);
  }
  log_library_log_file = fopen(file_path, "a");
  if (!log_library_log_file) {
    log_library_log_file = stderr;
  }
  UNLOCK();
}

void log_library_format_current_time(char *buffer, size_t buffer_size) {
  struct timespec ts;
  struct tm tm_info;

// Get the current time with nanoseconds
#if defined(_WIN32) || defined(_WIN64)
  SYSTEMTIME st;
  FILETIME ft;
  ULARGE_INTEGER ull;

  GetSystemTime(&st);
  SystemTimeToFileTime(&st, &ft);
  ull.LowPart = ft.dwLowDateTime;
  ull.HighPart = ft.dwHighDateTime;

  // Convert to UNIX epoch (seconds since 1970-01-01)
  time_t seconds = (ull.QuadPart - 116444736000000000ULL) / 10000000ULL;
  long nanoseconds = (ull.QuadPart % 10000000ULL) * 100;
  ts.tv_sec = seconds;
  ts.tv_nsec = nanoseconds;

  gmtime_s(&tm_info, &ts.tv_sec);// Thread-safe on Windows
#else
  clock_gettime(CLOCK_REALTIME, &ts);
  localtime_r(&ts.tv_sec, &tm_info);// Thread-safe on POSIX
#endif

  // Format the time into the buffer
  snprintf(buffer, buffer_size, "%04d-%02d-%02d %02d:%02d:%02d.%09ld",
           tm_info.tm_year + 1900,
           tm_info.tm_mon + 1,
           tm_info.tm_mday,
           tm_info.tm_hour,
           tm_info.tm_min,
           tm_info.tm_sec,
           ts.tv_nsec);
}

// Function to print log message
static inline void log_library_log_message(const char *color, const char *fmt, ...) {
  LOCK();
  va_list argptr;
  va_start(argptr, fmt);

  FILE *output = log_library_log_file ? log_library_log_file : stderr;
  int is_terminal = output == stderr;

  if (is_terminal) {
    fprintf(output, "%s", color);
    vfprintf(output, fmt, argptr);
    fprintf(output, "%s", COLOR_RESET);
  } else {
    vfprintf(output, fmt, argptr);
  }
  va_end(argptr);

  fflush(output);

  UNLOCK();
}

#define ___LOG___(color, fmt, level, path, ...)                                              \
  do {                                                                                       \
    char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];                             \
    log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE); \
    log_library_log_message(color, "%s [%s] [%s:%d] [%s] " fmt "\n",                         \
                            log_library_time_buffer, level, path, __LINE__, FUNC_NAME,       \
                            ##__VA_ARGS__);                                                  \
  } while (0)

#define LOGDEBUG(fmt, ...) ___LOG___(COLOR_BLUE, fmt, "DEBUG", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGINFO(fmt, ...) ___LOG___(COLOR_GREEN, fmt, "INFO", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGWARN(fmt, ...) ___LOG___(COLOR_YELLOW, fmt, "WARN", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) ___LOG___(COLOR_RED, fmt, "ERROR", __SHORT_FILE__, ##__VA_ARGS__)

#if defined(LOG_LIBRARY_LOG_LEVEL_ERROR)
#undef LOGDEBUG
#undef LOGINFO
#undef LOGWARN
#define LOGDEBUG(fmt, ...) ((void) 0)
#define LOGINFO(fmt, ...) ((void) 0)
#define LOGWARN(fmt, ...) ((void) 0)
#elif defined(LOG_LIBRARY_LOG_LEVEL_WARN)
#undef LOGDEBUG
#undef LOGINFO
#define LOGDEBUG(fmt, ...) ((void) 0)
#define LOGINFO(fmt, ...) ((void) 0)
#elif defined(LOG_LIBRARY_LOG_LEVEL_DEBUG)
#undef LOGINFO
#define LOGINFO(fmt, ...) ((void) 0)
#elif defined(LOG_LIBRARY_LOG_LEVEL_INFO)
#endif

#ifdef __cplusplus

static inline std::string log_library_form_exception(const char *short_file, int line, const char *func_name, const char *fmt, ...) {
  char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];
  log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE);
  std::string message = "[EXCEPTION]";
  message += " ";
  message += log_library_time_buffer;
  message += " ";
  message += "[";
  message += short_file;
  message += ":";
  message += std::to_string(line);
  message += "]";
  message += " ";
  message += "[";
  message += func_name;
  message += "]";
  message += " ";
  message += fmt;
  return message;
}

#define EXCEPTION(fmt, ...) (log_library_form_exception(__SHORT_FILE__, __LINE__, FUNC_NAME, fmt, ##__VA_ARGS__))

#endif

#endif// LOGGER_H
