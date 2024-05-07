#include "material_critical_amount.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

struct MaterialCriticalAmountRow {
    std::optional<std::int64_t> id;
    std::int64_t material_id;
    std::int64_t critical_amount;
};

std::vector<ColumnType> MaterialCriticalAmountTable::COLUMNS = {
    {"id",              SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}                                },
    {"material_id",     SqlType::INTEGER, {"NOT NULL", "REFERENCES \"materials\"(\"material_id\")"}},
    {"critical_amount", SqlType::INTEGER, {"NOT NULL"}                                             },
};

std::string MaterialCriticalAmountTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string MaterialCriticalAmountTable::formSelectRowQuery() {
    const auto return_str = fmt::format("SELECT * FROM {};", TABLE_NAME);
    return return_str;
}

std::string MaterialCriticalAmountTable::formInsertRowQuery(const MaterialCriticalAmountRow& row) {
    const auto return_str = fmt::format("INSERT INTO {} VALUES (NULL, {}, {});", TABLE_NAME,
                                        row.material_id, row.critical_amount);

    return return_str;
}

std::string
MaterialCriticalAmountTable::formUpdateRowQuery(std::int64_t id,
                                                const MaterialCriticalAmountRow& material) {
    const auto return_str = fmt::format("UPDATE {} SET "
                                        "critical_amount = '{}' "
                                        "WHERE material_id = {};",
                                        TABLE_NAME, material.critical_amount, id);
    return return_str;
}

std::string MaterialCriticalAmountTable::formDeleteRowQuery(std::int64_t id) {
    const auto return_str = fmt::format("DELETE FROM {} WHERE id = {};", TABLE_NAME, id);

    return return_str;
}
