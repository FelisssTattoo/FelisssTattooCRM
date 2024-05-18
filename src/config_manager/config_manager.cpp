#include "config_manager.h"

#include <felisss_logger/felisss_logger.h>

#include <nlohmann/json.hpp>

#include <fstream>

std::optional<ConfigManager::ConfigValues>
ConfigManager::getConfigValues(const std::string_view& config_pathname) {
    std::ifstream file_handler(config_pathname.data());

    ConfigManager::ConfigValues config_values{};
    nlohmann::json json_object;
    try {
        json_object = nlohmann::json::parse(file_handler);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return config_values;
    }

    try {
        if (json_object.contains("token")) {
            config_values.token = json_object.at("token");
        }

        if (json_object.contains("admin_pass")) {
            config_values.admin_pass = json_object.at("admin_pass");
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return config_values;
    }

    return config_values;
}
