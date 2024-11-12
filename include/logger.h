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
#define LOG_LIBRFARY_UINT_BUFFER_SIZE 12
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
static HANDLE log_library_mutex = NULL;
#define LOG_LIBRARY_LOCK()                                \
  do {                                                    \
    if (!log_library_mutex) {                             \
      log_library_mutex = CreateMutex(NULL, FALSE, NULL); \
    }                                                     \
    WaitForSingleObject(log_library_mutex, INFINITE);     \
  } while (0)
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

static inline const void SSTR(char *buffer, int size, unsigned int x) {
  snprintf(buffer, size, "%u", x);
}

static inline std::string log_library_form_exception(const char *short_file, unsigned int line, const char *LOG_LIBRARY_func_name, const char *fmt, ...) {
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
  char log_library_uint_buffer[LOG_LIBRFARY_UINT_BUFFER_SIZE];
  SSTR(log_library_uint_buffer, LOG_LIBRFARY_UINT_BUFFER_SIZE, line);
  message += log_library_uint_buffer;
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

#if (defined(_MSC_VER) && _MSC_VER >= 1900) || (defined(__cplusplus) && __cplusplus >= 201103L)

template<typename T>
struct log_library_is_container {
private:
  template<typename U>
  static constexpr bool test(typename U::const_iterator *) { return true; }

  template<typename>
  static constexpr bool test(...) { return false; }

public:
  static constexpr bool value = test<T>(nullptr);
};

template<typename T>
struct log_library_is_pair : std::false_type {};

template<typename T, typename U>
struct log_library_is_pair<std::pair<T, U>> : std::true_type {};

template<typename T>
struct log_library_is_string : std::false_type {};

template<>
struct log_library_is_string<std::string> : std::true_type {};

template<typename T>
static inline void log_library_print_element_wrapper(const T &element, std::ostringstream &oss);

template<typename T>
static inline void log_library_print_container(const T &container, std::ostringstream &oss);

static inline void log_library_print_element(const std::string &element, std::ostringstream &oss) {
  oss << "\"" << element << "\"";
}

template<typename T>
static inline typename std::enable_if<!log_library_is_pair<T>::value && !log_library_is_container<T>::value && !log_library_is_string<T>::value>::type
log_library_print_element(const T &element, std::ostringstream &oss) {
  oss << element;
}

template<typename T>
static inline typename std::enable_if<log_library_is_pair<T>::value>::type
log_library_print_element(const T &element, std::ostringstream &oss) {
  oss << "\"" << element.first << "\": ";
  log_library_print_element_wrapper(element.second, oss);
}

template<typename T>
static inline typename std::enable_if<log_library_is_container<T>::value && !log_library_is_string<T>::value>::type
log_library_print_element(const T &element, std::ostringstream &oss) {
  log_library_print_container(element, oss);
}

template<typename T>
static inline void log_library_print_element_wrapper(const T &element, std::ostringstream &oss) {
  log_library_print_element(element, oss);
}

template<typename T>
static inline void log_library_print_container(const T &container, std::ostringstream &oss) {
  oss << "{";
  bool first = true;
  for (const auto &element: container) {
    if (!first) oss << ", ";
    log_library_print_element_wrapper(element, oss);
    first = false;
  }
  oss << "}";
}

template<typename T>
static inline std::string log_library_get_container_string(const T &container) {
  std::ostringstream oss;
  log_library_print_container(container, oss);
  return oss.str();
}

template<typename T>
static inline std::string log_library_class_string(const T &value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

// Macros for logging standard containers and custom classes
#define STD_CONTAINER(container) ("[" + std::string(#container) + "] " + log_library_get_container_string(container))
#define CPP_CLASS(class_name) ("[" + std::string(#class_name) + "] {" + log_library_class_string(class_name) + "}")

#endif

#endif// __cplusplus

#endif// LOGGER_H
