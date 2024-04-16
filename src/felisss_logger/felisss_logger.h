#pragma once

#include <spdlog/spdlog.h>

#include <string_view>

class FelisssLogger {
public:
    static void init();

private:
    static constexpr std::string_view LOG_FILENAME = "logfile";
};
