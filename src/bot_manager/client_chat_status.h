#pragma once

#include <database_manager/database_manager.h>

#include <tgbot/tgbot.h>

class ClientChatStatus {
public:
    TgBot::InlineKeyboardMarkup::Ptr current_menu;

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

    void clearAllProperties();
};
