#pragma once

#include "database_manager_types.h"

#include <optional>
#include <string>

class TattooArtistsTable {
public:
    struct TattooArtistRow {
        std::optional<std::int64_t> tattoo_artist_id;
        std::int64_t user_id;
    };

    static std::string formCreateTableQuery();
    static std::string formInsertRowQuery(const TattooArtistRow& row);

    static constexpr std::string_view TABLE_NAME = "tattoo_artists";
    static std::vector<ColumnType> COLUMNS;
};
