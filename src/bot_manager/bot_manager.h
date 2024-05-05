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

    void sendMessage(const TgBot::Message::Ptr& recv_message, const std::string& message);
    void sendMenuWithMessage(const TgBot::Message::Ptr& recv_message,
                             const TgBot::GenericReply::Ptr& menu, const std::string& message);
    void editMenuWithMessage(const TgBot::Message::Ptr& recv_message,
                             const TgBot::InlineKeyboardMarkup::Ptr& menu,
                             const std::string& message);
    void sendCurrentMenu(const TgBot::Message::Ptr& recv_message);
    void editCurrentMenu(const TgBot::Message::Ptr& recv_message);

    void callbackOnStartCommand(const TgBot::Message::Ptr& message);
    void callbackOnMakeMeAdminCommand(const TgBot::Message::Ptr& message);
    void callbackOnMakeMeTattooArtistCommand(const TgBot::Message::Ptr& message);
    void callbackOnAnyMessage(const TgBot::Message::Ptr& message);
    void callbackOnCallbackQuery(const TgBot::CallbackQuery::Ptr& query);

    bool checkIfTelegramIdIsAdmin(std::int64_t telegram_id);
    bool checkIfTelegramIdIsTattooArtist(std::int64_t telegram_id);

    void updateChooseMaterialMenu();

    std::shared_ptr<ClientChatStatus> getClientChatStatus(const TgBot::Message::Ptr& message);
    void insertUserInTableIfNotExists(const TgBot::Message::Ptr& message);
    UsersTable::UserRow scrapUserDataFromMessage(const TgBot::Message::Ptr& message);
    std::optional<std::string> getMenuMessage(const TgBot::InlineKeyboardMarkup::Ptr& menu);
    std::string formUserInfoStrFromMessage(const TgBot::User::Ptr& user);

private:
    const std::string_view mToken;

    TgBot::Bot mBotHandler;
    TgBot::TgLongPoll mLongPoll;

    DatabaseManager mDatabaseManager;
    static constexpr std::string_view DATABASE_PATHNAME = "felisss.db";

    std::map<int64_t, std::shared_ptr<ClientChatStatus>> mClientChatStatuses;

    static TgBot::InlineKeyboardMarkup::Ptr mMainMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mMaterialsMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseMaterialMenu;

    static constexpr std::string_view PARSE_MODE      = "HTML";
    static constexpr std::string_view MATERIAL_PREFIX = "material_id_";
};