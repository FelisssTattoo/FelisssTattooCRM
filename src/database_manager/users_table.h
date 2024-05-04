#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class UsersTable {
public:
    struct UserRow {
        std::optional<std::int64_t> id;
        std::string name;
        std::optional<std::string> surname;
        std::optional<std::string> patronymic;
        std::optional<std::string> birthdate;
        std::optional<std::string> phone_number;
        std::optional<std::string> telegram;
        std::optional<std::int64_t> telegram_id;
        std::optional<std::string> instagram;
    };

    static std::string formCreateTableQuery();
    static std::string formInsertRowQuery(const UserRow& row);
    static std::string formSelectRowQuery();

    static constexpr std::string_view TABLE_NAME = "users";
    static std::vector<ColumnType> COLUMNS;
};
