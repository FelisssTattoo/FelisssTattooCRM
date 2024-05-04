#include "tattoo_artists_table.h"

#include "database_manager_tools.h"

#include <fmt/core.h>

std::vector<ColumnType> TattooArtistsTable::COLUMNS = {
    {"admin_id", SqlType::INTEGER, {"PRIMARY KEY", "UNIQUE"}                        },
    {"user_id",  SqlType::INTEGER, {"NOT NULL", "REFERENCES \"users\"(\"user_id\")"}}
};

std::string TattooArtistsTable::formCreateTableQuery() {
    return DatabaseManagerTools::formCreateTableQuery(TABLE_NAME.data(), COLUMNS);
}

std::string TattooArtistsTable::formInsertRowQuery(const TattooArtistRow& row) {
    const auto return_str = fmt::format("INSERT INTO {} VALUES (NULL, \"{}\");", TABLE_NAME,
                                        row.user_id);

    return return_str;
}
