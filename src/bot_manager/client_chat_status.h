#pragma once

#include <database_manager/database_manager.h>

struct ClientChatStatus {
    MaterialsTable::MaterialRow updating_material{};
    bool do_user_type_material_name        = false;
    bool do_user_specify_material_suffix   = false;
    bool do_user_specify_material_count    = false;
    bool do_user_update_material_count     = false;
    bool do_user_choose_to_modify_material = false;
    bool do_user_choose_to_delete_material = false;
};
