#pragma once

#include "client_chat_status.h"
#include "timer.h"

#include <database_manager/database_manager.h>
#include <felisss_tattoo/cmake_vars.h>

#include <tgbot/tgbot.h>

#include <memory>

class BotManager {
public:
    explicit BotManager(const std::string_view& token, const std::string_view& admin_pass);

    void poll();

    static std::string formUserInfoStr(const TgBot::User::Ptr& user);
    static std::string formUserInfoStr(const UsersTable::UserRow& user_row);
    static std::string formMaterialInfoStr(const MaterialsTable::MaterialRow& material_row);
    static std::string formSessionInfoStr(const SessionsTable::SessionRow& row,
                                          const UsersTable::UserRow& tattoo_artist,
                                          std::optional<UsersTable::UserRow>& customer);

private:
    void init();
    void initMenus();

    void sendMessage(std::int64_t telegram_id, const std::string& message);
    void sendMessage(const TgBot::Message::Ptr& recv_message, const std::string& message);
    void sendMessage(const TgBot::CallbackQuery::Ptr& query, const std::string& message);
    void sendMenuWithMessage(const TgBot::Message::Ptr& recv_message,
                             const TgBot::GenericReply::Ptr& menu, const std::string& message);
    void editMenuWithMessage(const TgBot::Message::Ptr& recv_message,
                             const TgBot::InlineKeyboardMarkup::Ptr& menu,
                             const std::string& message);
    void sendCurrentMenu(const TgBot::Message::Ptr& message);
    void editCurrentMenu(const TgBot::Message::Ptr& message);
    void sendCurrentMenu(const TgBot::CallbackQuery::Ptr& query);
    void editCurrentMenu(const TgBot::CallbackQuery::Ptr& query);

    void callbackOnStartCommand(const TgBot::Message::Ptr& message);
    void callbackOnMakeMeAdminCommand(const TgBot::Message::Ptr& message);
    void callbackOnMakeMeTattooArtistCommand(const TgBot::Message::Ptr& message);
    void callbackOnOwnerCommand(const TgBot::Message::Ptr& message);
    void callbackOnAnyMessage(const TgBot::Message::Ptr& message);
    void callbackOnCallbackQuery(const TgBot::CallbackQuery::Ptr& query);

    void sendMaterialAlarms();
    void scheduleCriticalAmountMessageIfNessessory();

    void sendSessionReminderIfNessessory();
    void sendSessionReminder(const SessionsTable::SessionRow& row);

    std::int64_t getTelegramIdFromQuery(const TgBot::CallbackQuery::Ptr& query);
    std::int64_t getTelegramIdFromQuery(const TgBot::Message::Ptr& message);

    std::shared_ptr<ClientChatStatus> getClientChatStatus(std::int64_t telegram_id);
    void insertUserInTableIfNotExists(const TgBot::Message::Ptr& message);
    UsersTable::UserRow scrapUserDataFromMessage(const TgBot::Message::Ptr& message);
    bool validateMessageAndSendErrorWithMenu(const TgBot::Message::Ptr& message);

private:
    const std::string_view mToken;
    const std::string_view mAdminPass;

    TgBot::Bot mBotHandler;
    TgBot::TgLongPoll mLongPoll;

    std::shared_ptr<DatabaseManager> mDatabase;
    static constexpr std::string_view DB_PATHNAME = DATABASE_PATHNAME;
    static constexpr std::string_view ERROR_MESSAGE
        = "На жаль сталась помилка. Спробуйте ще раз або зверніться до адміністратора";

    std::map<int64_t, std::shared_ptr<ClientChatStatus>> mClientChatStatuses;

    Timer mAlarmMessageTimer;
    int mAlarmMessageDelayMinutes = 1;

    Timer mSessionReminderTimer;

    static TgBot::ReplyKeyboardMarkup::Ptr mSkipMenu;

    static constexpr std::string_view PARSE_MODE = "";

    static constexpr std::string_view SKIP_BUTTON_TEXT = "Пропустити";
};
