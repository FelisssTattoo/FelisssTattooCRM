cmake_minimum_required(VERSION 3.20)

project(FelisssTattooCRM)

add_subdirectory(libs)

file(GLOB srcs "src/**")
add_executable(${PROJECT_NAME} ${srcs})

set(LIBS_LIST spdlog::spdlog_header_only sqlite TgBot)
target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})
