cmake_minimum_required(VERSION 3.20)

project(FelisssTattooCRM)

add_subdirectory(libs)
set(LIBS_LIST TgBot spdlog::spdlog_header_only)

file(GLOB srcs "src/**")
add_executable(${PROJECT_NAME} ${srcs})

target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})
