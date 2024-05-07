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
}
