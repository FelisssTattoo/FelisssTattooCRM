cmake_minimum_required(VERSION 3.20)

project(FelisssTattooCRM)

add_subdirectory(libs)
set(LIBS_LIST TgBot)

file(GLOB srcs "src/**")
add_executable(${PROJECT_NAME} ${srcs})

target_link_libraries(${PROJECT_NAME} ${LIBS_LIST})
