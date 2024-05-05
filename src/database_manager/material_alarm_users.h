#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class MaterialAlarmUsersTable {
public:
    struct MaterialAlarmUserRow {
        std::optional<std::int64_t> id;
        std::int64_t user_id;
    };

    static std::string formCreateTableQuery();
    static std::string formInsertRowQuery(const MaterialAlarmUserRow& row);
    static std::string formDeleteRowQuery(std::int64_t id);

    static constexpr std::string_view TABLE_NAME = "material_alarm_users";
    static std::vector<ColumnType> COLUMNS;
};
