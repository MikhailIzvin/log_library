cmake_minimum_required(VERSION 3.7)
project("semaphore_file_tracking" VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(LOG_LIBRARY_ENABLE_SEMAPHORE)
add_definitions(-DLOG_LIBRABRY_SEMAPHORE_NAME=\"semaphore_file_tracking\")

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    static_library
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/static_library/include
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/static_library)
