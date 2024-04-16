cmake_minimum_required(VERSION 3.20)

project(FelisssTattooCRM)

option(RUN_CLANG_TIDY "run clang-tidy on compile" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs)

if(RUN_CLANG_TIDY)
    message(STATUS "Compiling with clang-tidy...")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy-16")
endif(RUN_CLANG_TIDY)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/config.json)
    file(COPY ${CMAKE_SOURCE_DIR}/samples/config.json
        DESTINATION ${CMAKE_BINARY_DIR}
    )
endif()

file(GLOB_RECURSE srcs "src/**")
add_executable(${PROJECT_NAME} ${srcs})

set(LIBS_LIST
    SQLiteCpp
    TgBot
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    fmt::fmt
)

target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
