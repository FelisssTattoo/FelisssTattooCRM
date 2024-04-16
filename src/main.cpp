#include "felisss_logger/felisss_logger.h"

#include "config_manager/config_manager.h"
#include "database_manager/database_manager.h"

#include <tgbot/tgbot.h>

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

        if (*token == "<your_bot_token_here>") {
            SPDLOG_CRITICAL("Specify token in config.json");
            return EXIT_FAILURE;
        }

        spdlog::info("Token: {}", *token);

        TgBot::Bot bot(*token);
        bot.getEvents().onCommand("start", [&bot](const TgBot::Message::Ptr& message) {
            bot.getApi().sendMessage(message->chat->id, "Hi!");
        });
        bot.getEvents().onAnyMessage([&bot](const TgBot::Message::Ptr& message) {
            spdlog::info("User wrote {}", message->text.c_str());
            if (StringTools::startsWith(message->text, "/start")) {
                return;
            }
            bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        });

        spdlog::info("Bot username: {}", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll long_poll(bot);
        while (true) {
            spdlog::debug("Long poll started");
            long_poll.start();
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
