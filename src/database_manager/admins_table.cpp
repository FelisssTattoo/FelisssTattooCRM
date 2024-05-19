#include "admins_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> AdminsTable::COLUMNS = {
    {"user_id", SqlType::INTEGER, {"NOT NULL", "UNIQUE", "REFERENCES \"users\"(\"user_id\")"}}
};

std::string AdminsTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string AdminsTable::formInsertRowQuery(const AdminRow& row) {
    const auto return_str = fmt::format("INSERT INTO {} VALUES (\"{}\");", TABLE_NAME, row.user_id);

    return return_str;
}

std::string AdminsTable::formDeleteRowQuery(std::int64_t id) {
    const auto return_str = fmt::format("DELETE FROM {} WHERE user_id = {};", TABLE_NAME, id);

    return return_str;
}
