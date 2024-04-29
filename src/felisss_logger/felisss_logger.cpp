#include "felisss_logger.h"

#include <spdlog/sinks/daily_file_sink.h>

void FelisssLogger::init() {
    auto sink = std::make_shared<spdlog::sinks::daily_file_sink_st>(LOG_FILENAME.data(),
                                                                    ROTATION_HOUR, ROTATION_MINUTE);
    spdlog::default_logger()->sinks().push_back(sink);
    spdlog::info("FelisssTattooBot started");
}
