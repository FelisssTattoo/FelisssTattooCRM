#include "felisss_logger/felisss_logger.h"

#include "bot_manager/bot_manager.h"
#include "config_manager/config_manager.h"
#include "database_manager/database_manager.h"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

int main() {
    try {
        FelisssLogger::init();

        DatabaseManager database_manager("felisss.db");
        database_manager.addCustomer(
            {"Dmytro", "Shtrikker", "Yakovich", "15/08/2002", "0973074604"});

        auto config_values = ConfigManager::getConfigValues();
        auto token         = config_values.token;
        if (!token) {
            SPDLOG_CRITICAL("Token not specified in env");
            return EXIT_FAILURE;
        }

        spdlog::info("Token: {}", *token);
        if (*token == "<your_bot_token_here>") {
            SPDLOG_CRITICAL("Specify token in config.json");
            return EXIT_FAILURE;
        }

        BotManager bot(*token);
        while (true) {
            bot.poll();
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
