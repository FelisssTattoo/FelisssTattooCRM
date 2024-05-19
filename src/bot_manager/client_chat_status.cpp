#include "client_chat_status.h"

void ClientChatStatus::clearAllProperties() {
    do_user_type_material_name        = false;
    do_user_specify_material_suffix   = false;
    do_user_specify_material_count    = false;
    do_user_update_material_count     = false;
    do_user_choose_to_modify_material = false;
    do_user_choose_to_delete_material = false;

    do_user_choose_material_critical_amount_to_add    = false;
    do_user_choose_material_critical_amount_to_update = false;
    do_user_choose_material_critical_amount_to_delete = false;

    do_user_type_date               = false;
    do_user_choose_user_for_session = false;
    session_row                     = {};

    do_user_type_user_name         = false;
    do_user_type_user_surname      = false;
    do_user_type_user_patronymic   = false;
    do_user_type_user_birthdate    = false;
    do_user_type_user_phone_number = false;
    do_user_type_user_telegram     = false;
    do_user_type_user_instagram    = false;
    user_row                       = {};

    do_user_choose_user_to_delete = false;

    do_user_choose_user_rights_tattoo_artist_add    = false;
    do_user_choose_user_rights_tattoo_artist_delete = false;
    do_user_choose_user_rights_admin_add            = false;
    do_user_choose_user_rights_admin_delete         = false;
}
