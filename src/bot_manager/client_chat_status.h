#pragma once

#include <database_manager/database_manager.h>

#include <tgbot/tgbot.h>

#include <bitset>

enum class Menus : std::uint8_t {
    MAIN_MENU,
    MATERIALS_MENU,
    MATERIALS_DELETE_MATERIAL_MENU,
    MATERIALS_UPDATE_MATERIAL_MENU,
    MATERIALS_CHOOSE_USER_CRITICAL_ALARM_MENU,
    CRITICAL_MATERIALS_MENU,
    CRITICAL_MATERIALS_MENU_ADD,
    CRITICAL_MATERIALS_MENU_DELETE,
    CRITICAL_MATERIALS_MENU_UPDATE,
    SESSIONS_MENU,
    SESSIONS_MENU_ADD_SESSION_CHOOSE_TATTOO_ARTIST,
    SESSIONS_MENU_ADD_SESSION_CHOOSE_CUSTOMER,
    SESSIONS_MENU_DELETE_SESSION,
    USERS_MENU,
    USERS_MENU_DELETE_USER,
    USER_RIGHTS_MENU,
    USER_RIGHTS_MENU_ADD_TATTOO_ARTIST,
    USER_RIGHTS_MENU_DELETE_TATTOO_ARTIST,
    USER_RIGHTS_MENU_ADD_ADMIN,
    USER_RIGHTS_MENU_DELETE_ADMIN
};

enum class ChatStatuses : std::uint8_t {
    DO_USER_TYPE_MATERIAL_NAME,
    DO_USER_SPECIFY_MATERIAL_SUFFIX,
    DO_USER_SPECIFY_MATERIAL_COUNT,
    DO_USER_UPDATE_MATERIAL_COUNT,
    DO_USER_CHOOSE_TO_MODIFY_MATERIAL,
    DO_USER_CHOOSE_TO_DELETE_MATERIAL,

    DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_ADD,
    DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE,
    DO_USER_TYPE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE,
    DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_DELETE,

    DO_USER_TYPE_DATE,
    DO_USER_CHOOSE_USER_FOR_SESSION,

    DO_USER_TYPE_USER_NAME,
    DO_USER_TYPE_USER_SURNAME,

    DO_USER_CHOOSE_USER_TO_DELETE,

    DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_ADD,
    DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_DELETE,
    DO_USER_CHOOSE_USER_RIGHTS_ADMIN_ADD,
    DO_USER_CHOOSE_USER_RIGHTS_ADMIN_DELETE,
    CHAT_STATUSES_SIZE
};

class ClientChatStatus {
public:
    ClientChatStatus(std::int64_t telegram_id, std::weak_ptr<DatabaseManager> database);

    void clearAllProperties();

    void setMenu(Menus menu);
    TgBot::InlineKeyboardMarkup::Ptr getMenu();

    std::string getMenuMessage();
    void returnToPreviousMenu();

    MaterialsTable::MaterialRow updating_material{};
    SessionsTable::SessionRow session_row{};
    UsersTable::UserRow user_row{};

    std::bitset<static_cast<std::uint8_t>(ChatStatuses::CHAT_STATUSES_SIZE)>::reference
    operator[](ChatStatuses chat_status_flag);
    std::bitset<static_cast<std::uint8_t>(ChatStatuses::CHAT_STATUSES_SIZE)>::reference
    at(ChatStatuses chat_status_flag);

private:
    void updateCurrentMenu();

    void setMainMenu();
    void setMaterialsMenu();
    void setMaterialsDeleteMaterialMenu();
    void setMaterialsUpdateMaterialMenu();
    void setMaterialsChooseUserCriticalAlarmMenu();
    void setCriticalMaterialsMenu();
    void setCriticalMaterialsAddMenu();
    void setCriticalMaterialsDeleteMenu();
    void setCriticalMaterialsUpdateMenu();
    void setSessionsMenu();
    void setSessionsAddSessionChooseTattooArtistMenu();
    void setSessionsAddSessionChooseCustomerMenu();
    void setSessionsDeleteSessionMenu();
    void setUsersMenu();
    void setUsersDeleteUserMenu();
    void setUserRightsMenu();
    void setUserRightsAddTattooArtistMenu();
    void setUserRightsDeleteTattooArtistMenu();
    void setUserRightsAddAdminMenu();
    void setUserRightsDeleteAdminMenu();

    const std::int64_t mTelegramId;

    std::weak_ptr<DatabaseManager> mDatabase;

    Menus mCurrentMenu;
    TgBot::InlineKeyboardMarkup::Ptr mMenu;

    static TgBot::InlineKeyboardButton::Ptr mBackButton;

    std::bitset<static_cast<std::uint8_t>(ChatStatuses::CHAT_STATUSES_SIZE)> mChatStatuses;
};
