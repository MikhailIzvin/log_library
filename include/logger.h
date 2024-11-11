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

#define LOG_LIBRFARY_TIME_BUFFER_SIZE 30
#ifdef LOG_LIBRARY_PRETTY_FUNCTION
#define LOG_LIBRARY_FUNC_NAME __PRETTY_FUNCTION__
#else
#define LOG_LIBRARY_FUNC_NAME __func__
#endif
#define LOG_LIBRARY_LINE __LINE__

#ifndef LOG_LIBRARY_DISABLE_COLORS
#define COLOR_RESET "\033[0m"
#define COLOR_BLUE "\033[34m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"
#else
#define COLOR_RESET ""
#define COLOR_BLUE ""
#define COLOR_GREEN ""
#define COLOR_YELLOW ""
#define COLOR_RED ""
#endif

// Define a mutex for thread safety
#if defined(_WIN32) || defined(_WIN64)
static HANDLE log_library_mutex = CreateMutex(NULL, FALSE, NULL);
#define LOG_LIBRARY_LOCK() WaitForSingleObject(log_library_mutex, INFINITE)
#define LOG_LIBRARY_UNLOCK() ReleaseMutex(log_library_mutex)
#define LOG_LIBRARY_SHORT_FILE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
static pthread_mutex_t log_library_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOG_LIBRARY_LOCK() pthread_mutex_lock(&log_library_mutex)
#define LOG_LIBRARY_UNLOCK() pthread_mutex_unlock(&log_library_mutex)
#define LOG_LIBRARY_SHORT_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


// Static log file pointer, defaults to stderr
static FILE *log_library_log_file = NULL;
static unsigned int log_library_log_size = 0;
static unsigned int log_library_log_max_size = 0;

typedef void (*log_library_callback)(void *userdata);
static log_library_callback log_library_max_file_size_callback = NULL;
static void *log_library_userdata = NULL;

// Safe functions
static inline void log_library_set_log_file(const char *file_path);
static inline void log_library_set_log_max_size(unsigned int max_size);
static inline unsigned int log_library_get_log_size();
static inline void log_library_close_log_file();
static inline void log_library_set_max_file_size_callback(log_library_callback callback, void *userdata);
static inline void log_library_flush_log();

// Unlocked functions
static inline void log_library_set_log_file_unlocked(const char *file_path);
static inline void log_library_set_log_max_size_unlocked(unsigned int max_size);
static inline unsigned int log_library_get_log_size_unlocked();
static inline void log_library_close_log_file_unlocked();
static inline void log_library_set_max_file_size_callback_unlocked(log_library_callback callback, void *userdata);
static inline void log_library_flush_log_unlocked();

// Private functions
static inline void log_library_format_current_time(char *buffer, size_t buffer_size);
static inline void log_library_log_message(const char *color, const char *fmt, ...);

// Sets the log file. If not set, logs default to stderr.
static inline void log_library_set_log_file(const char *file_path) {
  LOG_LIBRARY_LOCK();
  log_library_set_log_file_unlocked(file_path);
  LOG_LIBRARY_UNLOCK();
}

static inline void log_library_set_log_file_unlocked(const char *file_path) {
  if (log_library_log_file && log_library_log_file != stderr) {
    fclose(log_library_log_file);
    log_library_log_size = 0;
  }
  log_library_log_file = fopen(file_path, "a");
  if (!log_library_log_file) {
    log_library_log_file = stderr;
  } else {
    fseek(log_library_log_file, 0, SEEK_END);
    log_library_log_size = ftell(log_library_log_file);
    fseek(log_library_log_file, 0, SEEK_SET);
  }
}

static inline void log_library_set_log_max_size(unsigned int max_size) {
  LOG_LIBRARY_LOCK();
  log_library_set_log_max_size_unlocked(max_size);
  LOG_LIBRARY_UNLOCK();
}

static inline void log_library_set_log_max_size_unlocked(unsigned int max_size) {
  log_library_log_max_size = max_size;
}


static inline unsigned int log_library_get_log_size() {
  LOG_LIBRARY_LOCK();
  unsigned int size = log_library_get_log_size_unlocked();
  LOG_LIBRARY_UNLOCK();
  return size;
}

static inline unsigned int log_library_get_log_size_unlocked() {
  return log_library_log_size;
}

static inline void log_library_close_log_file() {
  LOG_LIBRARY_LOCK();
  log_library_close_log_file_unlocked();
  LOG_LIBRARY_UNLOCK();
}

static inline void log_library_close_log_file_unlocked() {
  if (log_library_log_file && log_library_log_file != stderr) {
    fclose(log_library_log_file);
    log_library_log_file = NULL;
    log_library_log_size = 0;
  }
}

static inline void log_library_set_max_file_size_callback(log_library_callback callback, void *userdata) {
  LOG_LIBRARY_LOCK();
  log_library_set_max_file_size_callback_unlocked(callback, userdata);
  LOG_LIBRARY_UNLOCK();
}

static inline void log_library_set_max_file_size_callback_unlocked(log_library_callback callback, void *userdata) {
  log_library_max_file_size_callback = callback;
  log_library_userdata = userdata;
}

static inline void log_library_format_current_time(char *buffer, size_t buffer_size) {
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
  LOG_LIBRARY_LOCK();
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
    log_library_log_size = ftell(output);
    if (log_library_log_max_size != 0 && log_library_log_size >= log_library_log_max_size) {
      if (log_library_max_file_size_callback) {
        log_library_max_file_size_callback(log_library_userdata);
      }
    }
  }
  va_end(argptr);

#ifndef LOG_LIBRARY_DISABLE_FLUSH
  fflush(output);
#endif

  LOG_LIBRARY_UNLOCK();
}

static inline void log_library_flush_log() {
  LOG_LIBRARY_LOCK();
  log_library_flush_log_unlocked();
  LOG_LIBRARY_UNLOCK();
}

static inline void log_library_flush_log_unlocked() {
  FILE *output = log_library_log_file ? log_library_log_file : stderr;
  fflush(output);
  int is_terminal = output == stderr;
  if (!is_terminal) {
    log_library_log_size = ftell(output);
  }
}


#ifndef LOG_LIBRARY_TAG_SUPPORT

#define ___LOG___(color, fmt, level, path, ...)                                                            \
  do {                                                                                                     \
    char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];                                           \
    log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE);               \
    log_library_log_message(color, "%s [%s] [%s:%d] [%s] " fmt "\n",                                       \
                            log_library_time_buffer, level, path, LOG_LIBRARY_LINE, LOG_LIBRARY_FUNC_NAME, \
                            ##__VA_ARGS__);                                                                \
  } while (0)

#ifdef LOG_LIBRARY_LOG_SIMPLE
#undef ___LOG___
#define ___LOG___(color, fmt, level, path, ...)                                                         \
  do {                                                                                                  \
    char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];                                        \
    log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE);            \
    log_library_log_message(color, "%s [%s] " fmt "\n", log_library_time_buffer, level, ##__VA_ARGS__); \
  } while (0)
#endif

#define LOGDEBUG(fmt, ...) ___LOG___(COLOR_BLUE, fmt, "DEBUG", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)
#define LOGINFO(fmt, ...) ___LOG___(COLOR_GREEN, fmt, "INFO", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)
#define LOGWARN(fmt, ...) ___LOG___(COLOR_YELLOW, fmt, "WARN", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) ___LOG___(COLOR_RED, fmt, "ERROR", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)

#else

#define ___LOG___(color, fmt, tag, level, path, ...)                                                            \
  do {                                                                                                          \
    char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];                                                \
    log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE);                    \
    log_library_log_message(color, "%s [%s] [%s] [%s:%d] [%s] " fmt "\n",                                       \
                            log_library_time_buffer, tag, level, path, LOG_LIBRARY_LINE, LOG_LIBRARY_FUNC_NAME, \
                            ##__VA_ARGS__);                                                                     \
  } while (0)

#ifdef LOG_LIBRARY_LOG_SIMPLE
#undef ___LOG___
#define ___LOG___(color, fmt, tag, level, path, ...)                                                              \
  do {                                                                                                            \
    char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];                                                  \
    log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE);                      \
    log_library_log_message(color, "%s [%s] [%s] " fmt "\n", log_library_time_buffer, tag, level, ##__VA_ARGS__); \
  } while (0)
#endif

#define LOGDEBUG(tag, fmt, ...) ___LOG___(COLOR_BLUE, fmt, tag, "DEBUG", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)
#define LOGINFO(tag, fmt, ...) ___LOG___(COLOR_GREEN, fmt, tag, "INFO", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)
#define LOGWARN(tag, fmt, ...) ___LOG___(COLOR_YELLOW, fmt, tag, "WARN", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)
#define LOGERROR(tag, fmt, ...) ___LOG___(COLOR_RED, fmt, tag, "ERROR", LOG_LIBRARY_SHORT_FILE, ##__VA_ARGS__)

#endif

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
#include <sstream>
#include <stdexcept>
#include <string>


#if __cplusplus < 201103L
#define SSTR(x) static_cast<std::ostringstream &>(         \
                  (std::ostringstream() << std::dec << x)) \
                  .str()
#else
#define SSTR(x) std::to_string(x)
#endif

static inline std::string log_library_form_exception(const char *short_file, int line, const char *LOG_LIBRARY_func_name, const char *fmt, ...) {
  char log_library_time_buffer[LOG_LIBRFARY_TIME_BUFFER_SIZE];
  log_library_format_current_time(log_library_time_buffer, LOG_LIBRFARY_TIME_BUFFER_SIZE);
  std::string message = "[EXCEPTION]";
  message += " ";
  message += log_library_time_buffer;
  message += " ";
#ifndef LOG_LIBRARY_LOG_SIMPLE
  message += "[";
  message += short_file;
  message += ":";
  message += SSTR(line);
  message += "]";
  message += " ";
  message += "[";
  message += LOG_LIBRARY_func_name;
  message += "]";
  message += " ";
#endif
  message += fmt;
  return message;
}

#define EXCEPTION(fmt, ...) (log_library_form_exception(LOG_LIBRARY_SHORT_FILE, LOG_LIBRARY_LINE, LOG_LIBRARY_FUNC_NAME, fmt, ##__VA_ARGS__))

#if __cplusplus >= 201103L

#define LOG_LIBRARY_STD_MAX_ELEMENTS 100

template<typename T>
static inline std::string log_library_to_string(const T &value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

template<typename T>
static inline bool log_library_is_fundamental(const T &value) {
  return std::is_fundamental<T>::value;
}

#define LOG_LIBRARY_CPP_CLASS(class_name) ("\n{\n  " + log_library_to_string(class_name) + "\n}")

template<typename T>
static inline std::string log_library_form_std_container(const char *name, const T &container, size_t max_elements = LOG_LIBRARY_STD_MAX_ELEMENTS) {
  std::string message = "[";
  message += name;
  message += "] = {";
  size_t count = 0;
  for (auto it = container.begin(); it != container.end(); ++it) {
    if (!log_library_is_fundamental(*it)) {
      message += LOG_LIBRARY_CPP_CLASS(*it);
    } else {
      message += log_library_to_string(*it);
    }
    if (std::next(it) != container.end())
      message += ", ";
    if (max_elements != -1 && ++count >= max_elements) {
      message += "...";
      break;
    }
  }
  message += "}";
  return message;
}

#define STD_CONTAINER(container) (log_library_form_std_container(#container, container))
#define STD_CONTAINER_MAX(container, max_elements) (log_library_form_std_container(#container, container, max_elements))
#define CPP_CLASS(class_name) ("[" + std::string(#class_name) + "]" + LOG_LIBRARY_CPP_CLASS(class_name))

#endif

#endif

#endif// LOGGER_H
