#include "materials_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> MaterialsTable::COLUMNS = {
    {"material_id", SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}},
    {"name",        SqlType::TEXT,    {"NOT NULL"}             },
    {"count",       SqlType::REAL,    {"NOT NULL"}             },
    {"suffix",      SqlType::TEXT,    {}                       }
};

std::string MaterialsTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string MaterialsTable::formUpdateCountByIdQuery(size_t count, size_t id) {
    const auto return_str = fmt::format("UPDATE {} SET count = {} WHERE material_id = {};",
                                        TABLE_NAME, count, id);
    return return_str;
}

std::string MaterialsTable::formInsertRowQuery(const MaterialRow& row) {
    const auto return_str = fmt::format(
        "INSERT INTO {} VALUES (NULL, \"{}\", {}, {});", TABLE_NAME, row.name, row.count,
        (row.suffix) ? (fmt::format("\"{}\"", *row.suffix)) : ("NULL"));

    return return_str;
}

std::string MaterialsTable::formSelectRowQuery() {
    const auto return_str = fmt::format("SELECT * FROM {};", TABLE_NAME);
    return return_str;
}

std::string MaterialsTable::formSelectRowByNameQuery(const std::string& name) {
    const auto return_str = fmt::format("SELECT * FROM {} WHERE name == \"{}\";", TABLE_NAME, name);
    return return_str;
}

std::string MaterialsTable::formSelectRowByIdQuery(size_t id) {
    const auto return_str = fmt::format("SELECT * FROM {} WHERE material_id == \"{}\";", TABLE_NAME,
                                        id);
    return return_str;
}
