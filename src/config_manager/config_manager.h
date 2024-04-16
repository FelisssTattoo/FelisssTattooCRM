#pragma once

#include <optional>
#include <string>

class ConfigManager {
public:
    struct ConfigValues {
        std::optional<std::string> token;
    };

public:
    static ConfigValues getConfigValues();

private:
    static constexpr std::string_view mConfigPathname = "config.json";
};
