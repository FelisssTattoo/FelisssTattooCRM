#pragma once

#include "admins_table.h"
#include "materials_table.h"
#include "users_table.h"

#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseManager {
public:
    explicit DatabaseManager(std::string_view db_pathname);

    void addCustomer(const UsersTable::UserRow& user_row);
    std::optional<UsersTable::UserRow> getUserByTelegramId(std::int64_t telegram_id);
    std::vector<UsersTable::UserRow> getAdmins();
    void addAdmin(const UsersTable::UserRow& user_row);

    bool addMaterial(const MaterialsTable::MaterialRow& material_row);
    bool updateMaterialCountById(size_t new_count, size_t id);
    std::optional<MaterialsTable::MaterialRow> getMaterialByName(const std::string& name);
    std::optional<MaterialsTable::MaterialRow> getMaterialById(size_t id);
    std::vector<MaterialsTable::MaterialRow> getMaterials();

private:
    void initCustomersTable();
    void initMaterialsTable();
    void initAdminsTable();

    UsersTable::UserRow getUserRowFromStatement(SQLite::Statement& statement);
    MaterialsTable::MaterialRow getMaterialRowFromStatement(SQLite::Statement& statement);

private:
    std::string_view mDbPathname;
    SQLite::Database mDbHandler;
};
