#pragma once

#include <felisss_tattoo/cmake_vars.h>
#include <spdlog/spdlog.h>

#include <string_view>

class FelisssLogger {
public:
    static void init();

private:
    static constexpr std::string_view LOG_FILENAME = LOGS_PATHNAME;
    static constexpr int ROTATION_HOUR             = 23;
    static constexpr int ROTATION_MINUTE           = 59;
};
