#include "sessions_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> SessionsTable::COLUMNS = {
    {"id",               SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}                        },
    {"date_time",        SqlType::TEXT,    {"NOT NULL"}                                     },
    {"tattoo_artist_id", SqlType::INTEGER, {"NOT NULL", "REFERENCES \"users\"(\"user_id\")"}},
    {"user_id",          SqlType::INTEGER, {"REFERENCES \"users\"(\"user_id\")"}            }
};

std::string SessionsTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string SessionsTable::formSelectRowQuery() {
    const auto return_str = fmt::format("SELECT * FROM {};", TABLE_NAME);
    return return_str;
}

std::string SessionsTable::formInsertRowQuery(const SessionRow& row) {
    const auto return_str = fmt::format(
        "INSERT INTO {} VALUES (NULL, \"{}\", {}, {});", TABLE_NAME, row.date_time,
        row.tattoo_artist_id, (row.user_id) ? (fmt::format("\"{}\"", *row.user_id)) : ("NULL"));

    return return_str;
}

std::string SessionsTable::formUpdateRowQuery(std::int64_t id, const SessionRow& row) {
    const auto return_str = fmt::format(
        "UPDATE {} SET "
        "date_time = '{}', "
        "tattoo_artist_id = '{}', "
        "user_id = {} "
        "WHERE material_id = {};",
        TABLE_NAME, row.date_time, row.tattoo_artist_id,
        (row.user_id) ? (fmt::format("\"{}\"", *row.user_id)) : ("NULL"), id);
    return return_str;
}

std::string SessionsTable::formDeleteRowQuery(std::int64_t id) {
    const auto return_str = fmt::format("DELETE FROM {} WHERE id = {};", TABLE_NAME, id);

    return return_str;
}
