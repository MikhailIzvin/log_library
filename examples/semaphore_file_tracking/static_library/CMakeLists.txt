cmake_minimum_required(VERSION 3.7)
project("static_library" VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(LOG_LIBRARY_TAG_SUPPORT)

add_library(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/library.cc
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
