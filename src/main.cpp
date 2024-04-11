#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>
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

    if (token_str == nullptr) {
        SPDLOG_CRITICAL("Token not specified in env");
        return EXIT_FAILURE;
    }

    const std::string TOKEN(token_str);
    spdlog::info("Token: {}", TOKEN.c_str());

    TgBot::Bot bot(TOKEN);
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

    signal(SIGINT, [](int /*s*/) {
        spdlog::info("SIGINT got");
        exit(EXIT_SUCCESS);
    });

    try {
        spdlog::info("Bot username: {}", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll long_poll(bot);
        while (true) {
            spdlog::debug("Long poll started");
            long_poll.start();
        }
    } catch (std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }

    return EXIT_SUCCESS;
}

void initLogger() {
    static constexpr std::string_view LOG_FILENAME = "logfile";
    static constexpr int ROTATION_HOUR             = 23;
    static constexpr int ROTATION_MINUTE           = 59;
    spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(
        LOG_FILENAME.data(), ROTATION_HOUR, ROTATION_MINUTE));
}
