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
#endif

#define TIME_BUFFER_SIZE 30

// Console colors
#define COLOR_RESET "\033[0m"
#define COLOR_BLUE "\033[34m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"

// Define a mutex for thread safety
#if defined(_WIN32) || defined(_WIN64)
static HANDLE mutex = NULL;
#define LOCK() WaitForSingleObject(mutex, INFINITE)
#define UNLOCK() ReleaseMutex(mutex)
#define __SHORT_FILE__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK() pthread_mutex_lock(&mutex)
#define UNLOCK() pthread_mutex_unlock(&mutex)
#define __SHORT_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef __PRETTY_FUNCTION__
#define FUNC_NAME __PRETTY_FUNCTION__
#else
#define FUNC_NAME __func__
#endif

// Static log file pointer, defaults to stderr
static FILE *log_file = NULL;

// Sets the log file. If not set, logs default to stderr.
static inline void set_log_file(const char *file_path) {
  LOCK();
  if (log_file && log_file != stderr) {
    fclose(log_file);
  }
  log_file = fopen(file_path, "a");
  if (!log_file) {
    log_file = stderr;
  }
  UNLOCK();
}

void format_current_time(char *buffer, size_t buffer_size) {
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
static inline void log_message(const char *color, const char *fmt, ...) {
  LOCK();
  va_list argptr;
  va_start(argptr, fmt);

  FILE *output = log_file ? log_file : stderr;
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

#define ___LOG___(color, fmt, level, path, ...)                            \
  do {                                                                     \
    char time_log_library_buffer[TIME_BUFFER_SIZE];                        \
    format_current_time(time_log_library_buffer, TIME_BUFFER_SIZE);        \
    log_message(color, "%s [%s] [%s:%d] [%s] " fmt "\n",                   \
                time_log_library_buffer, level, path, __LINE__, FUNC_NAME, \
                ##__VA_ARGS__);                                            \
  } while (0)

#define LOGDEBUG(fmt, ...) ___LOG___(COLOR_BLUE, fmt, "DEBUG", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGINFO(fmt, ...) ___LOG___(COLOR_GREEN, fmt, "INFO", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGWARN(fmt, ...) ___LOG___(COLOR_YELLOW, fmt, "WARN", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) ___LOG___(COLOR_RED, fmt, "ERROR", __SHORT_FILE__, ##__VA_ARGS__)

// Initialize mutex for Windows platform
#if defined(_WIN32) || defined(_WIN64)
static inline void initialize_logger() {
  if (!mutex) {
    mutex = CreateMutex(NULL, FALSE, NULL);
  }
}
#define INIT_LOGGER() initialize_logger()
#else
#define INIT_LOGGER() ((void) 0)
#endif

#ifdef __cplusplus
#include <stdexcept>
#include <string>

#define EXCEPTION(msg) (std::string(__SHORT_FILE__) + ":" + std::to_string(__LINE__) + " " + FUNC_NAME + " " + msg)

#define throw_runtime_error(msg) throw std::runtime_error(EXCEPTION(msg))

#define throw_invalid_argument(msg) throw std::invalid_argument(EXCEPTION(msg))

#define ___LOGEXCEPTION___(color, level, fmt)                       \
  do {                                                              \
    char time_log_library_buffer[TIME_BUFFER_SIZE];                 \
    format_current_time(time_log_library_buffer, TIME_BUFFER_SIZE); \
    log_message(color, "%s [%s] %s %s\n",                           \
                time_log_library_buffer, level, fmt);               \
  } while (0)

#define LOGERROREXCEPTION(what) ___LOGEXCEPTION___(COLOR_BLUE, "DEBUG", what)
#define LOGDEBUGEXCEPTION(what) ___LOGEXCEPTION___(COLOR_RED, "ERROR", what)


#endif

#endif// LOGGER_H
