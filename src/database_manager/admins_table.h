#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class AdminsTable {
public:
    struct AdminRow {
        std::optional<std::int64_t> admin_id;
        std::int64_t user_id;
    };

    static std::string formCreateTableQuery();
    static std::string formInsertRowQuery(const AdminRow& row);

    static constexpr std::string_view TABLE_NAME = "admins";
    static std::vector<ColumnType> COLUMNS;
};
