#include "material_alarm_users.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> MaterialAlarmUsersTable::COLUMNS = {
    {"id",      SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}                        },
    {"user_id", SqlType::INTEGER, {"NOT NULL", "REFERENCES \"users\"(\"user_id\")"}}
};

std::string MaterialAlarmUsersTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string MaterialAlarmUsersTable::formInsertRowQuery(const MaterialAlarmUserRow& row) {
    const auto return_str = fmt::format("INSERT INTO {} VALUES (NULL, \"{}\");", TABLE_NAME,
                                        row.user_id);

    return return_str;
}

std::string MaterialAlarmUsersTable::formDeleteRowQuery(std::int64_t id) {
    const auto return_str = fmt::format("DELETE FROM {} WHERE id = {};", TABLE_NAME, id);

    return return_str;
}
