#pragma once

#include <database_manager/database_manager.h>

#include <tgbot/tgbot.h>

enum class Menus {
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

class ClientChatStatus {
public:
    ClientChatStatus(std::int64_t telegram_id, std::weak_ptr<DatabaseManager> database);

    void clearAllProperties();

    void setMenu(Menus menu);
    TgBot::InlineKeyboardMarkup::Ptr getMenu();

    std::string getMenuMessage();
    void returnToPreviousMenu();

    MaterialsTable::MaterialRow updating_material{};
    bool do_user_type_material_name        = false;
    bool do_user_specify_material_suffix   = false;
    bool do_user_specify_material_count    = false;
    bool do_user_update_material_count     = false;
    bool do_user_choose_to_modify_material = false;
    bool do_user_choose_to_delete_material = false;

    bool do_user_choose_material_critical_amount_to_add    = false;
    bool do_user_choose_material_critical_amount_to_update = false;
    bool do_user_type_material_critical_amount_to_update   = false;
    bool do_user_choose_material_critical_amount_to_delete = false;

    bool do_user_type_date               = false;
    bool do_user_choose_user_for_session = false;
    SessionsTable::SessionRow session_row{};

    bool do_user_type_user_name         = false;
    bool do_user_type_user_surname      = false;
    bool do_user_type_user_patronymic   = false;
    bool do_user_type_user_birthdate    = false;
    bool do_user_type_user_phone_number = false;
    bool do_user_type_user_telegram     = false;
    bool do_user_type_user_instagram    = false;
    UsersTable::UserRow user_row{};

    bool do_user_choose_user_to_delete = false;

    bool do_user_choose_user_rights_tattoo_artist_add    = false;
    bool do_user_choose_user_rights_tattoo_artist_delete = false;
    bool do_user_choose_user_rights_admin_add            = false;
    bool do_user_choose_user_rights_admin_delete         = false;

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
};
