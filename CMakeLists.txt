cmake_minimum_required(VERSION 3.7)
project("logger" VERSION 1.0.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


option(PRETTY_FUNCTION "Enable pretty function names in log output" OFF)
option(DISABLE_COLORS "Disable colored log output" OFF)
option(LOG_LEVEL_ERROR "Set log level to ERROR" OFF)
option(LOG_LEVEL_DEBUG "Set log level to DEBUG" OFF)
option(LOG_LEVEL_WARN "Set log level to WARN" OFF)
option(LOG_LEVEL_INFO "Set log level to INFO" OFF)
option(LOG_SIMPLE "Enable simple log output" OFF)
option(CUSTOM_LOG_FILE "Set custom log file" OFF)
option(CUSTOM_LOG_DIR "Set custom log directory" OFF)
option(DISABLE_FLUSH "Disable flush after each log message" OFF)

if (CUSTOM_LOG_FILE)
  set(LOG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/log.txt")
else()
  set(LOG_FILE "${CMAKE_CURRENT_BINARY_DIR}/log.txt")
endif()

if (CUSTOM_LOG_DIR)
  set(LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
else()
  set(LOG_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

if(PRETTY_FUNCTION)
  message("Enable pretty function names in log output")
  add_compile_definitions(LOG_LIBRARY_PRETTY_FUNCTION)
endif()

if(DISABLE_COLORS)
  message("Disable colored log output")
  add_compile_definitions(LOG_LIBRARY_DISABLE_COLORS)
endif()

if(LOG_LEVEL_ERROR)
  message("Set log level to ERROR")
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_ERROR)
endif()

if(LOG_LEVEL_DEBUG)
  message("Set log level to DEBUG")
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_DEBUG)
endif()

if(LOG_LEVEL_WARN)
  message("Set log level to WARN")
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_WARN)
endif()

if(LOG_LEVEL_INFO)
  message("Set log level to INFO")
  add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_INFO)
endif()

if(LOG_SIMPLE)
  message("Enable simple log output")
  add_compile_definitions(LOG_LIBRARY_LOG_SIMPLE)
endif()

if(DISABLE_FLUSH)
  message("Disable flush after each log message")
  add_compile_definitions(LOG_LIBRARY_DISABLE_FLUSH)
endif()

set(EXAMPLE_DIR examples)

configure_file (config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_DIR})
