#include "tattoo_artists_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> TattooArtistsTable::COLUMNS = {
    {"user_id", SqlType::INTEGER, {"NOT NULL", "UNIQUE", "REFERENCES \"users\"(\"user_id\")"}}
};

std::string TattooArtistsTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string TattooArtistsTable::formInsertRowQuery(const TattooArtistRow& row) {
    const auto return_str = fmt::format("INSERT INTO {} VALUES (\"{}\");", TABLE_NAME, row.user_id);

    return return_str;
}

std::string TattooArtistsTable::formDeleteRowQuery(std::int64_t id) {
    const auto return_str = fmt::format("DELETE FROM {} WHERE user_id = {};", TABLE_NAME, id);

    return return_str;
}
