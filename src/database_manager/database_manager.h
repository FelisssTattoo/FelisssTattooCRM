#pragma once

#include "admins_table.h"
#include "material_alarm_users.h"
#include "material_critical_amount.h"
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

    bool addMaterialAlarmUser(const UsersTable::UserRow& user_row);
    bool deleteMaterialAlarmUserById(std::int64_t id);
    std::vector<UsersTable::UserRow> getMaterialAlarmUsers();

    bool addMaterialCriticalAmount(const MaterialsTable::MaterialRow& material_row,
                                   std::int64_t critical_amount);
    bool updateMaterialCriticalAmountById(
        std::int64_t id,
        const MaterialCriticalAmountTable::MaterialCriticalAmountRow& material_row);
    bool deleteMaterialCriticalAmount(std::int64_t id);
    bool deleteMaterialCriticalAmountByMaterialId(std::int64_t material_id);
    std::vector<MaterialCriticalAmountTable::MaterialCriticalAmountRow> getMaterialCriticalAmount();
    std::optional<MaterialCriticalAmountTable::MaterialCriticalAmountRow>
    getMaterialCriticalAmountByMaterialId(std::int64_t material_id);

    bool addMaterial(const MaterialsTable::MaterialRow& material_row);
    bool updateMaterialCountById(std::int64_t id, const MaterialsTable::MaterialRow& material_row);
    bool deleteMaterialById(std::int64_t id);
    std::vector<MaterialsTable::MaterialRow> getMaterials();
    std::optional<MaterialsTable::MaterialRow> getMaterialByName(const std::string& name);
    std::optional<MaterialsTable::MaterialRow> getMaterialById(size_t id);
    std::vector<MaterialsTable::MaterialRow> getCriticalMaterials();

private:
    void initUsersTable();
    void initMaterialsTable();
    void initAdminsTable();
    void initTattooArtistsTable();
    void initMaterialAlarmUsersTable();
    void initMaterialCriticalAmountTable();

    UsersTable::UserRow getUserRowFromStatement(SQLite::Statement& statement);
    MaterialsTable::MaterialRow getMaterialRowFromStatement(SQLite::Statement& statement);
    MaterialCriticalAmountTable::MaterialCriticalAmountRow
    getMaterialCriticalAmountRowFromStatement(SQLite::Statement& statement);

private:
    std::string_view mDbPathname;
    SQLite::Database mDbHandler;

    bool isAdminsVectorUpdated;
    bool isMaterialAlarmUsersVectorUpdated;
    bool isTattooArtistsVectorUpdated;
    bool isUsersVectorUpdated;
    bool isMaterialsVectorUpdated;
    bool isMaterialCriticalAmountVectorUpdated;
};
