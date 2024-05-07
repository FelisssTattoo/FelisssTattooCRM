#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class MaterialCriticalAmountTable {
public:
    struct MaterialCriticalAmountRow {
        std::optional<std::int64_t> id;
        std::int64_t material_id;
        std::int64_t critical_amount;
    };

    static std::string formCreateTableQuery();
    static std::string formSelectRowQuery();
    static std::string formInsertRowQuery(const MaterialCriticalAmountRow& row);
    static std::string formUpdateRowQuery(std::int64_t id,
                                          const MaterialCriticalAmountRow& material);
    static std::string formDeleteRowQuery(std::int64_t id);

    static constexpr std::string_view TABLE_NAME = "material_critical_amount";
    static std::vector<ColumnType> COLUMNS;
};
