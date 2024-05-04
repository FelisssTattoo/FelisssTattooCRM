#pragma once

#include "admins_table.h"
#include "materials_table.h"
#include "tattoo_artists_table.h"
#include "users_table.h"

#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseManager {
public:
    explicit DatabaseManager(std::string_view db_pathname);

    bool addUser(const UsersTable::UserRow& user_row);
    std::vector<UsersTable::UserRow> getUsers();
    std::optional<UsersTable::UserRow> getUserByTelegramId(std::int64_t telegram_id);

    bool addAdmin(const UsersTable::UserRow& user_row);
    std::vector<UsersTable::UserRow> getAdmins();

    bool addTattooArtist(const UsersTable::UserRow& user_row);
    std::vector<UsersTable::UserRow> getTattooArtists();

    bool addMaterial(const MaterialsTable::MaterialRow& material_row);
    bool updateMaterialCountById(std::int64_t id, const MaterialsTable::MaterialRow& material_row);
    bool deleteMaterialById(std::int64_t id);
    std::vector<MaterialsTable::MaterialRow> getMaterials();
    std::optional<MaterialsTable::MaterialRow> getMaterialByName(const std::string& name);
    std::optional<MaterialsTable::MaterialRow> getMaterialById(size_t id);

private:
    void initUsersTable();
    void initMaterialsTable();
    void initAdminsTable();
    void initTattooArtistsTable();

    UsersTable::UserRow getUserRowFromStatement(SQLite::Statement& statement);
    MaterialsTable::MaterialRow getMaterialRowFromStatement(SQLite::Statement& statement);

private:
    std::string_view mDbPathname;
    SQLite::Database mDbHandler;

    bool isAdminsVectorUpdated;
    bool isTattooArtistsVectorUpdated;
    bool isUsersVectorUpdated;
    bool isMaterialsVectorUpdated;
};
