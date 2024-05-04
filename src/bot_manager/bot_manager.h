#pragma once

#include "client_chat_status.h"

#include <database_manager/database_manager.h>

#include <tgbot/tgbot.h>

#include <memory>

class BotManager {
public:
    explicit BotManager(const std::string& token);

    void poll();

private:
    void init();
    void initMenus();

    void callbackOnStartCommand(const TgBot::Message::Ptr& message);
    void callbackOnMakeMeAdminCommand(const TgBot::Message::Ptr& message);
    void callbackOnMakeMeTattooArtistCommand(const TgBot::Message::Ptr& message);
    void callbackOnAnyMessage(const TgBot::Message::Ptr& message);
    void callbackOnCallbackQuery(const TgBot::CallbackQuery::Ptr& query);

    std::shared_ptr<ClientChatStatus> getClientChatStatus(int64_t client_id);
    void insertUserInTableIfNotExists(const TgBot::Message::Ptr& message);
    UsersTable::UserRow scrapUserDataFromMessage(const TgBot::Message::Ptr& message);

    bool checkIfTelegramIdIsAdmin(std::int64_t telegram_id);
    bool checkIfTelegramIdIsTattooArtist(std::int64_t telegram_id);

private:
    const std::string_view mToken;

    TgBot::Bot mBotHandler;
    TgBot::TgLongPoll mLongPoll;

    DatabaseManager mDatabaseManager;
    static constexpr std::string_view DATABASE_PATHNAME = "felisss.db";

    std::map<int64_t, std::shared_ptr<ClientChatStatus>> mClientChatStatuses;

    static TgBot::InlineKeyboardMarkup::Ptr mAdminMainMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mMaterialsMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseMaterialMenu;
};
