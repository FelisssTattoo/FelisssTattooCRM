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

private:
    void init();
    void initMenus();

    void sendMessage(std::int64_t telegram_id, const std::string& message);
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
    void updateChooseMaterialCriticalAmountToAdd();
    void updateChooseMaterialCriticalAmountToUpdateDelete();
    void updateChooseMaterialAlarmUserMenu();
    void updateChooseTattooArtistMenu();
    void updateChooseUserMenu();
    void updateChooseSessionMenu();

    std::string formUserInfoStr(const TgBot::User::Ptr& user);
    std::string formUserInfoStr(const UsersTable::UserRow& user_row);
    std::string formMaterialInfoStr(const MaterialsTable::MaterialRow& material_row);
    std::string formSessionInfoStr(const SessionsTable::SessionRow& row);

    void sendMaterialAlarms();
    void scheduleCriticalAmountMessageIfNessessory();

    void sendSessionReminderIfNessessory();
    void sendSessionReminder(const SessionsTable::SessionRow& row);

    std::shared_ptr<ClientChatStatus> getClientChatStatus(const TgBot::Message::Ptr& message);
    void insertUserInTableIfNotExists(const TgBot::Message::Ptr& message);
    UsersTable::UserRow scrapUserDataFromMessage(const TgBot::Message::Ptr& message);
    std::optional<std::string> getMenuMessage(const TgBot::InlineKeyboardMarkup::Ptr& menu);
    TgBot::InlineKeyboardMarkup::Ptr
    returnPreviousMenu(const TgBot::InlineKeyboardMarkup::Ptr& current_menu);
    bool validateMessageAndSendErrorWithMenu(const TgBot::Message::Ptr& message);

private:
    const std::string_view mToken;
    const std::string_view mAdminPass;

    TgBot::Bot mBotHandler;
    TgBot::TgLongPoll mLongPoll;

    DatabaseManager mDatabaseManager;
    static constexpr std::string_view DB_PATHNAME = DATABASE_PATHNAME;
    static constexpr std::string_view ERROR_MESSAGE
        = "На жаль сталась помилка. Спробуйте ще раз або зверніться до адміністратора";

    std::map<int64_t, std::shared_ptr<ClientChatStatus>> mClientChatStatuses;

    Timer mAlarmMessageTimer;
    int mAlarmMessageDelayMinutes = 1;

    Timer mSessionReminderTimer;

    static TgBot::InlineKeyboardButton::Ptr mBackButton;

    static TgBot::ReplyKeyboardMarkup::Ptr mSkipMenu;

    static TgBot::InlineKeyboardMarkup::Ptr mMainMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mSessionsMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseTattooArtistMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseUserMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseSessionMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mUsersMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mMaterialsMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseMaterialMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseMaterialAlarmUserMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mConfigureMaterialCriticalAmountMenu;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseCriticalAmountMaterialMenuToAdd;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseCriticalAmountMaterialMenuToUpdateDelete;
    static TgBot::InlineKeyboardMarkup::Ptr mChooseCriticalAmountMaterialMenu;

    static constexpr std::string_view PARSE_MODE = "";

    static constexpr std::string_view SKIP_BUTTON_TEXT = "Пропустити";

    static constexpr std::string_view CHOOSE_MATERIAL_PREFIX            = "material_id_";
    static constexpr std::string_view CHOOSE_MATERIAL_ALARM_USER_PREFIX = "material_alarm_user_id_";
    static constexpr std::string_view CHOOSE_TATTOO_ARTIST_PREFIX       = "tattoo_artist_id_";
    static constexpr std::string_view CHOOSE_USER_PREFIX                = "user_id_";
    static constexpr std::string_view CHOOSE_SESSION_PREFIX             = "session_id_";
};
