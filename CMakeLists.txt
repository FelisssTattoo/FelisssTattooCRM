cmake_minimum_required(VERSION 3.20)

project(FelisssTattooCRM)

option(RUN_CLANG_TIDY "run clang-tidy on compile" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_subdirectory(libs)

if(RUN_CLANG_TIDY)
    message(STATUS "Compiling with clang-tidy...")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy-16")
endif(RUN_CLANG_TIDY)

file(GLOB_RECURSE srcs "src/**")
add_executable(${PROJECT_NAME} ${srcs})

set(LIBS_LIST spdlog::spdlog_header_only SQLiteCpp TgBot fmt-header-only)
target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})
