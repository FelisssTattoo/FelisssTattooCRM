#include "users_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> UsersTable::COLUMNS = {
    {"user_id",      SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}},
    {"name",         SqlType::TEXT,    {"NOT NULL"}             },
    {"surname",      SqlType::TEXT,    {}                       },
    {"patronymic",   SqlType::TEXT,    {}                       },
    {"birthdate",    SqlType::TEXT,    {}                       },
    {"phone_number", SqlType::TEXT,    {}                       },
    {"telegram",     SqlType::TEXT,    {}                       },
    {"telegram_id",  SqlType::INTEGER, {}                       },
    {"instagram",    SqlType::TEXT,    {}                       }
};

std::string UsersTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string UsersTable::formInsertRowQuery(const UserRow& row) {
    const auto return_str = fmt::format(
        "INSERT INTO {} VALUES (NULL, \"{}\", {}, {}, {}, {}, {}, {}, {});", TABLE_NAME, row.name,
        (row.surname) ? (fmt::format("\"{}\"", *row.surname)) : ("NULL"),
        (row.patronymic) ? (fmt::format("\"{}\"", *row.patronymic)) : ("NULL"),
        (row.birthdate) ? (fmt::format("\"{}\"", *row.birthdate)) : ("NULL"),
        (row.phone_number) ? (fmt::format("\"{}\"", *row.phone_number)) : ("NULL"),
        (row.telegram) ? (fmt::format("\"{}\"", *row.telegram)) : ("NULL"),
        (row.telegram_id) ? (fmt::format("\"{}\"", *row.telegram_id)) : ("NULL"),
        (row.instagram) ? (fmt::format("\"{}\"", *row.instagram)) : ("NULL"));

    return return_str;
}

std::string UsersTable::formSelectRowQuery() {
    const auto return_str = fmt::format("SELECT * FROM {};", TABLE_NAME);

    return return_str;
}

std::string UsersTable::formDeleteRowQuery(std::int64_t id) {
    const auto return_str = fmt::format("DELETE FROM {} WHERE user_id = {};", TABLE_NAME, id);

    return return_str;
}
