#include "admins_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> AdminsTable::COLUMNS = {
    {"admin_id", SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}                        },
    {"user_id",  SqlType::INTEGER, {"NOT NULL", "REFERENCES \"users\"(\"user_id\")"}}
};

std::string AdminsTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string AdminsTable::formInsertRowQuery(const AdminRow& row) {
    const auto return_str = fmt::format("INSERT INTO {} VALUES (NULL, \"{}\");", TABLE_NAME,
                                        row.user_id);

    return return_str;
}
