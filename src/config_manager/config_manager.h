#pragma once

#include <optional>
#include <string>

class ConfigManager {
public:
    struct ConfigValues {
        std::optional<std::string> token;
        std::optional<std::string> admin_pass;
    };

public:
    static std::optional<ConfigValues> getConfigValues(const std::string_view& config_pathname);
};
