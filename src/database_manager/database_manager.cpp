#include "database_manager.h"

#include <felisss_logger/felisss_logger.h>

#include <fmt/core.h>

DatabaseManager::DatabaseManager(std::string_view db_pathname) :
    mDbPathname(db_pathname),
    mDbHandler(mDbPathname, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    initCustomersTable();
    initMaterialsTable();
    initAdminsTable();
}

void DatabaseManager::addCustomer(const UsersTable::UserRow& user_row) {
    try {
        mDbHandler.exec(UsersTable::formInsertRowQuery(user_row));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

std::optional<UsersTable::UserRow> DatabaseManager::getUserByTelegramId(std::int64_t telegram_id) {

    SQLite::Statement query(mDbHandler, UsersTable::formSelectByTelegramIdQuery(telegram_id));

    try {
        query.executeStep();

        return getUserRowFromStatement(query);
    } catch (const std::exception& e) {
        SPDLOG_WARN("{}", e.what());
    }
    return {};
}

std::vector<UsersTable::UserRow> DatabaseManager::getAdmins() {
    const auto return_str = fmt::format("SELECT u.* "
                                        "FROM users u "
                                        "INNER JOIN admins a ON u.user_id = a.user_id;",
                                        UsersTable::TABLE_NAME, AdminsTable::TABLE_NAME);
    SQLite::Statement query(mDbHandler, return_str);
    std::vector<UsersTable::UserRow> return_vec;
    while (query.executeStep()) {
        return_vec.push_back(getUserRowFromStatement(query));
    }

    return return_vec;
}

void DatabaseManager::addAdmin(const UsersTable::UserRow& user_row) {
    AdminsTable::AdminRow admin_row = {.user_id = user_row.id.value()};
    try {
        mDbHandler.exec(AdminsTable::formInsertRowQuery(admin_row));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

bool DatabaseManager::addMaterial(const MaterialsTable::MaterialRow& material_row) {
    try {
        return mDbHandler.exec(MaterialsTable::formInsertRowQuery(material_row));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::updateMaterialCountById(size_t new_count, size_t id) {
    try {
        return mDbHandler.exec(MaterialsTable::formUpdateCountByIdQuery(new_count, id));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::optional<MaterialsTable::MaterialRow>
DatabaseManager::getMaterialByName(const std::string& name) {
    SQLite::Statement query(mDbHandler, MaterialsTable::formSelectRowByNameQuery(name));

    try {
        query.executeStep();

        return getMaterialRowFromStatement(query);
    } catch (const std::exception& e) {
        SPDLOG_WARN("{}", e.what());
        return {};
    }
}

std::optional<MaterialsTable::MaterialRow> DatabaseManager::getMaterialById(size_t id) {
    SQLite::Statement query(mDbHandler, MaterialsTable::formSelectRowByIdQuery(id));

    try {
        query.executeStep();

        return getMaterialRowFromStatement(query);
    } catch (const std::exception& e) {
        SPDLOG_WARN("{}", e.what());
        return {};
    }
}

std::vector<MaterialsTable::MaterialRow> DatabaseManager::getMaterials() {
    SQLite::Statement query(mDbHandler, MaterialsTable::formSelectRowQuery());
    std::vector<MaterialsTable::MaterialRow> return_vec;
    while (query.executeStep()) {
        return_vec.push_back(getMaterialRowFromStatement(query));
    }

    return return_vec;
}

void DatabaseManager::initCustomersTable() {
    try {
        mDbHandler.exec(UsersTable::formCreateTableQuery());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void DatabaseManager::initMaterialsTable() {
    try {
        mDbHandler.exec(MaterialsTable::formCreateTableQuery());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void DatabaseManager::initAdminsTable() {
    try {
        mDbHandler.exec(AdminsTable::formCreateTableQuery());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

UsersTable::UserRow DatabaseManager::getUserRowFromStatement(SQLite::Statement& statement) {
    UsersTable::UserRow user_row{};

    if (!statement.getColumn(0).isNull()) {
        user_row.id = statement.getColumn(0).getInt64();
    }
    if (!statement.getColumn(1).isNull()) {
        user_row.name = statement.getColumn(1).getString();
    }
    if (!statement.getColumn(2).isNull()) {
        user_row.surname = statement.getColumn(2).getString();
    }
    if (!statement.getColumn(3).isNull()) {
        user_row.patronymic = statement.getColumn(3).getString();
    }
    if (!statement.getColumn(4).isNull()) {
        user_row.birthdate = statement.getColumn(4).getString();
    }
    if (!statement.getColumn(5).isNull()) {
        user_row.phone_number = statement.getColumn(5).getString();
    }
    if (!statement.getColumn(6).isNull()) {
        user_row.telegram = statement.getColumn(6).getString();
    }
    if (!statement.getColumn(7).isNull()) {
        user_row.telegram_id = statement.getColumn(7).getInt64();
    }
    if (!statement.getColumn(8).isNull()) {
        user_row.instagram = statement.getColumn(8).getString();
    }

    return user_row;
}

MaterialsTable::MaterialRow
DatabaseManager::getMaterialRowFromStatement(SQLite::Statement& statement) {
    MaterialsTable::MaterialRow material_row{};

    if (!statement.getColumn(0).isNull()) {
        material_row.id = statement.getColumn(0).getInt64();
    }
    if (!statement.getColumn(1).isNull()) {
        material_row.name = statement.getColumn(1).getString();
    }
    if (!statement.getColumn(2).isNull()) {
        material_row.count = statement.getColumn(2).getDouble();
    }
    if (!statement.getColumn(3).isNull()) {
        material_row.suffix = statement.getColumn(3).getString();
    }

    return material_row;
}
