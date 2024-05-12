#include "config_manager.h"

#include <felisss_logger/felisss_logger.h>

#include <nlohmann/json.hpp>

#include <fstream>

std::optional<ConfigManager::ConfigValues>
ConfigManager::getConfigValues(const std::string_view& config_pathname) {
    std::ifstream file_handler(config_pathname.data());

    nlohmann::json json_object;
    try {
        json_object = nlohmann::json::parse(file_handler);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return {};
    }

    ConfigManager::ConfigValues config_values{};
    if (json_object.contains("token")) {
        config_values.token = json_object.at("token");
    }

    return config_values;
}
