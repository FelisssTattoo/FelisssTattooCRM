#pragma once

#include <tgbot/tgbot.h>

class BotManager {
public:
    explicit BotManager(const std::string& token);

    void poll();

private:
    void init();

private:
    const std::string_view mToken;

    TgBot::Bot mBotHandler;
    TgBot::TgLongPoll mLongPoll;
};
