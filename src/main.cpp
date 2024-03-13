#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <tgbot/tgbot.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

using namespace std;
using namespace TgBot;

static constexpr string_view LOG_FILENAME = "logfile";

int main() {
    spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(LOG_FILENAME.data(), 23, 59));
    char* token_str = getenv("TOKEN");

    if (token_str == NULL) {
        SPDLOG_CRITICAL("Token not specified in env");
        exit(EXIT_FAILURE);
    }

    string token(token_str);
    spdlog::info("Token: {}", token.c_str());

    Bot bot(token);
    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
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

        TgLongPoll longPoll(bot);
        while (true) {
            spdlog::debug("Long poll started");
            longPoll.start();
        }
    } catch (exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }

    return EXIT_SUCCESS;
}
