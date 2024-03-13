#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <tgbot/tgbot.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

void initLogger();

int main() {
    initLogger();

    char* token_str = getenv("TOKEN");

    if (token_str == NULL) {
        SPDLOG_CRITICAL("Token not specified in env");
        exit(EXIT_FAILURE);
    }

    std::string token(token_str);
    spdlog::info("Token: {}", token.c_str());

    TgBot::Bot bot(token);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        spdlog::info("User wrote {}", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    signal(SIGINT, [](int s) {
        spdlog::info("SIGINT got");
        exit(EXIT_SUCCESS);
    });

    try {
        spdlog::info("Bot username: {}", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            spdlog::debug("Long poll started");
            longPoll.start();
        }
    } catch (std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }

    return EXIT_SUCCESS;
}

void initLogger() {
    static constexpr std::string_view LOG_FILENAME = "logfile";
    spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(LOG_FILENAME.data(), 23, 59));
}
