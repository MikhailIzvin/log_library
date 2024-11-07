cmake_minimum_required(VERSION 3.7)
project("executable" VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 98)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(LOG_LIBRARY_PRETTY_FUNCTION)
add_compile_definitions(LOG_LIBRARY_LOG_LEVEL_INFO)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc
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

# add_dependencies(${PROJECT_NAME})