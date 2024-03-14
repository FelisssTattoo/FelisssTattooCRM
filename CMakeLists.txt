cmake_minimum_required(VERSION 3.20)

project(FelisssTattooCRM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_subdirectory(libs)

file(GLOB srcs "src/**")
add_executable(${PROJECT_NAME} ${srcs})

set(LIBS_LIST spdlog::spdlog_header_only SQLiteCpp TgBot)
target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})
