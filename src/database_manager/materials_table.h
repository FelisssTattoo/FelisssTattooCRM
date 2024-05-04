#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class MaterialsTable {
public:
    struct MaterialRow {
        std::optional<std::int64_t> id;
        std::string name;
        double count;
        std::optional<std::string> suffix;
    };

    static std::string formCreateTableQuery();
    static std::string formSelectRowQuery();
    static std::string formInsertRowQuery(const MaterialRow& row);
    static std::string formUpdateRowQuery(std::int64_t id, const MaterialRow& material);
    static std::string formDeleteRowQuery(std::int64_t id);

    static constexpr std::string_view TABLE_NAME = "materials";
    static std::vector<ColumnType> COLUMNS;
};
