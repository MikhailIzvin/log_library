cmake_minimum_required(VERSION 3.7)
project("logger" VERSION 1.0.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


option(LOG_LIBRARY_PRETTY_FUNCTION "Enable pretty function names in log output" OFF)
option(LOG_LIBRARY_DISABLE_COLORS "Disable colored log output" OFF)
option(LOG_LIBRARY_LOG_LEVEL_ERROR "Set log level to ERROR" OFF)
option(LOG_LIBRARY_LOG_LEVEL_DEBUG "Set log level to DEBUG" OFF)
option(LOG_LIBRARY_LOG_LEVEL_WARN "Set log level to WARN" OFF)
option(LOG_LIBRARY_LOG_LEVEL_INFO "Set log level to INFO" OFF)
option(LOG_LIBRARY_LOG_SIMPLE "Enable simple log output" OFF)

if(LOG_LIBRARY_PRETTY_FUNCTION)
  add_compile_definitions(LOG_LIBRARY_PRETTY_FUNCTION)
endif()

if(LOG_LIBRARY_DISABLE_COLORS)
  add_compile_definitions(LOG_LIBRARY_DISABLE_COLORS)
endif()

if(LOG_LIBRARY_LOG_LEVEL_ERROR)
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_ERROR)
endif()

if(LOG_LIBRARY_LOG_LEVEL_DEBUG)
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_DEBUG)
endif()

if(LOG_LIBRARY_LOG_LEVEL_WARN)
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_WARN)
endif()

if(LOG_LIBRARY_LOG_LEVEL_INFO)
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_INFO)
endif()

if(LOG_LIBRARY_LOG_SIMPLE)
  add_compile_definitions(LOG_LIBRARY_LOG_SIMPLE)
endif()

include_directories(include)

add_subdirectory(examples)
