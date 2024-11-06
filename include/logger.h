#pragma once
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

static void getFormattedTime(char *const p, int sz) {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(p, sz, "%Y-%m-%d %H:%M:%S", timeinfo);
}

static void mylog(const char *fmt, ...) {
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stderr, fmt, argptr);// log to stderr
  va_end(argptr);
  fflush(stderr);
}

#ifdef _WIN32
#define __SHORT_FILE__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __SHORT_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define ___LOG___(color, fmt, level, path, ...)                            \
  do {                                                                     \
    /* using local var and using a long name to avoid conflict*/           \
    char LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___[24];                  \
    getFormattedTime(LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___,          \
                     sizeof(LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___)); \
    mylog("%s%s [%s] [%s:%d] [%s] " fmt "%s\n",                            \
          color,                                                           \
          LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___,                     \
          level,                                                           \
          path,                                                            \
          __LINE__,                                                        \
          __PRETTY_FUNCTION__,                                             \
          ##__VA_ARGS__,                                                   \
          KNRM);                                                           \
  } while (0)

#define LOGDEBUG(fmt, ...) ___LOG___(KBLU, fmt, "DEBUG", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGINFO(fmt, ...) ___LOG___(KGRN, fmt, "INFO", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGWARN(fmt, ...) ___LOG___(KYEL, fmt, "WARN", __SHORT_FILE__, ##__VA_ARGS__)
#define LOGERROR(fmt, ...) ___LOG___(KRED, fmt, "ERROR", __SHORT_FILE__, ##__VA_ARGS__)

#ifdef __cplusplus
#include <stdexcept>
#include <string>

#define EXCEPTION(msg) ('[' + std::string(__SHORT_FILE__) + ':' + std::to_string(__LINE__) + ']' + ' ' + '[' + std::string(__PRETTY_FUNCTION__) + ']' + ' ' + std::string(msg))

#define throw_runtime_error(msg) throw std::runtime_error(EXCEPTION(msg))

#define throw_invalid_argument(msg) throw std::invalid_argument(EXCEPTION(msg))

#define ___LOGEXCEPTION___(color, level, what)                             \
  do {                                                                     \
    /* using local var and using a long name to avoid conflict*/           \
    char LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___[24];                  \
    getFormattedTime(LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___,          \
                     sizeof(LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___)); \
    mylog("%s%s [%s] %s %s"                                                \
          "\n",                                                            \
          color,                                                           \
          LAgGV3nzJsTholGvGL2eTNXmhsqYe___xxooxxoo___,                     \
          level,                                                           \
          what,                                                            \
          KNRM);                                                           \
  } while (0)

#define LOGERROREXCEPTION(what) ___LOGEXCEPTION___(KBLU, "DEBUG", what)
#define LOGDEBUGEXCEPTION(what) ___LOGEXCEPTION___(KRED, "ERROR", what)
#endif
