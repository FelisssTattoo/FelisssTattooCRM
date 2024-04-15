#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class CustomersTable {
public:
    struct CustomersRow {
        std::string name;
        std::optional<std::string> surname;
        std::optional<std::string> patronymic;
        std::optional<std::string> birthdate;
        std::optional<std::string> phone_number;
    };

public:
    static std::string formCreateTableQuery();
    static std::string formInsertRowQuery(const CustomersRow& row);

private:
    static constexpr std::string_view mTableName = "customers";
};
