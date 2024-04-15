#include "customers_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

namespace {
const std::vector<ColumnType> CUSTOMERS_COLUMNS = {
    {"customer_id",  SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}},
    {"name",         SqlType::TEXT,    {"NOT NULL"}             },
    {"surname",      SqlType::TEXT,    {}                       },
    {"patronymic",   SqlType::TEXT,    {}                       },
    {"birthdate",    SqlType::TEXT,    {}                       },
    {"phone_number", SqlType::TEXT,    {}                       }
};
}

std::string CustomersTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(mTableName.data(), CUSTOMERS_COLUMNS);
}

std::string CustomersTable::formInsertRowQuery(const CustomersRow& row) {
    auto return_str = fmt::format(
        "INSERT INTO {} VALUES (NULL, \"{}\", {}, {}, {}, {});", mTableName, row.name,
        (row.surname) ? (fmt::format("\"{}\"", *row.surname)) : ("NULL"),
        (row.patronymic) ? (fmt::format("\"{}\"", *row.patronymic)) : ("NULL"),
        (row.birthdate) ? (fmt::format("\"{}\"", *row.birthdate)) : ("NULL"),
        (row.phone_number) ? (fmt::format("\"{}\"", *row.phone_number)) : ("NULL"));

    return return_str;
}
