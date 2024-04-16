#include "felisss_logger.h"

#include <spdlog/sinks/daily_file_sink.h>

void FelisssLogger::init() {
    static constexpr int ROTATION_HOUR   = 23;
    static constexpr int ROTATION_MINUTE = 59;
    spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(
        LOG_FILENAME.data(), ROTATION_HOUR, ROTATION_MINUTE));
    spdlog::info("FelisssTattooBot started");
}
