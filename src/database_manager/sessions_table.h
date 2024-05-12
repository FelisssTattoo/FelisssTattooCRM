#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class SessionsTable {
public:
    struct SessionRow {
        std::optional<std::int64_t> id;
        std::string date_time;
        std::int64_t tattoo_artist_id;
        std::optional<std::int64_t> user_id;
    };

    static std::string formCreateTableQuery();
    static std::string formSelectRowQuery();
    static std::string formInsertRowQuery(const SessionRow& row);
    static std::string formUpdateRowQuery(std::int64_t id, const SessionRow& row);
    static std::string formDeleteRowQuery(std::int64_t id);

    static constexpr std::string_view TABLE_NAME = "sessions_table";
    static std::vector<ColumnType> COLUMNS;
};
