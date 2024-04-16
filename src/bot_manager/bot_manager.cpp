#include "bot_manager.h"

#include <felisss_logger/felisss_logger.h>

BotManager::BotManager(const std::string& token) :
    mToken(token), mBotHandler(token), mLongPoll(mBotHandler) {
    init();
}

void BotManager::poll() {
    mLongPoll.start();
}

void BotManager::init() {
    mBotHandler.getEvents().onCommand("start", [&](const TgBot::Message::Ptr& message) {
        mBotHandler.getApi().sendMessage(message->chat->id, "Hi!");
    });
    mBotHandler.getEvents().onAnyMessage([&](const TgBot::Message::Ptr& message) {
        spdlog::info("User wrote {}", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        mBotHandler.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });
}