#pragma once

#include <database_manager/database_manager.h>

struct ClientChatStatus {
    bool do_user_type_material_name      = false;
    bool do_user_specify_material_suffix = false;
    bool do_user_specify_material_count  = false;
    MaterialsTable::MaterialRow typed_material_row{};
    bool do_user_update_material_count = false;
    MaterialsTable::MaterialRow updating_material{};
};
