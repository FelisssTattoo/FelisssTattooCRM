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

set(CMAKE_INSTALL_PREFIX "")
set(IN_FILE "${PROJECT_SOURCE_DIR}/generated_files/cmake_vars.h.in")
set(GENERATED_FILE_PATH "${PROJECT_BINARY_DIR}/generated_files")  
set(OUT_FILE "${GENERATED_FILE_PATH}/felisss_tattoo/cmake_vars.h")
configure_file("${IN_FILE}" "${OUT_FILE}" @ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src" "${GENERATED_FILE_PATH}")

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "samples/felisss_tattoo" DESTINATION ${CMAKE_INSTALL_SYSCONFDIR})
install(FILES "samples/felisss_tattoo.service" DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/systemd/system")
