#include "database_manager.h"

#include "database_manager_tools.h"

#include <felisss_logger/felisss_logger.h>

#include <fmt/core.h>

DatabaseManager::DatabaseManager(std::string_view db_pathname) :
    mDbPathname(db_pathname),
    mDbHandler(mDbPathname, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE),
    isAdminsVectorUpdated(false),
    isTattooArtistsVectorUpdated(false),
    isUsersVectorUpdated(false),
    isMaterialsVectorUpdated(false),
    isMaterialAlarmUsersVectorUpdated(false),
    isMaterialCriticalAmountVectorUpdated(false),
    isSessionsVectorUpdated(false) {
    initUsersTable();
    initMaterialsTable();
    initAdminsTable();
    initTattooArtistsTable();
    initMaterialAlarmUsersTable();
    initMaterialCriticalAmountTable();
    initSessionsTable();
}

bool DatabaseManager::addUser(const UsersTable::UserRow& user_row) {
    try {
        const bool is_okay = mDbHandler.exec(UsersTable::formInsertRowQuery(user_row));
        if (is_okay) {
            isUsersVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::vector<UsersTable::UserRow> DatabaseManager::getUsers() {
    static std::vector<UsersTable::UserRow> users_vector;

    if (!isUsersVectorUpdated) {
        users_vector.clear();

        SQLite::Statement query(mDbHandler, UsersTable::formSelectRowQuery());
        while (query.executeStep()) {
            users_vector.push_back(getUserRowFromStatement(query));
        }

        isUsersVectorUpdated = true;
    }

    return users_vector;
}

std::optional<UsersTable::UserRow> DatabaseManager::getUserById(std::int64_t id) {
    const auto users = getUsers();

    const auto user = std::find_if(users.begin(), users.end(), [id](const auto& user_row) {
        return (user_row.id == id);
    });

    if (user != users.end()) {
        return *user;
    }

    return {};
}

std::optional<UsersTable::UserRow> DatabaseManager::getUserByTelegramId(std::int64_t telegram_id) {
    const auto users = getUsers();

    const auto user = std::find_if(users.begin(), users.end(), [telegram_id](const auto& user_row) {
        return (user_row.telegram_id == telegram_id);
    });

    if (user != users.end()) {
        return *user;
    }

    return {};
}

bool DatabaseManager::addAdmin(const UsersTable::UserRow& user_row) {
    AdminsTable::AdminRow admin_row = {.user_id = user_row.id.value()};
    try {
        const bool is_okay = mDbHandler.exec(AdminsTable::formInsertRowQuery(admin_row));
        if (is_okay) {
            isAdminsVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::vector<UsersTable::UserRow> DatabaseManager::getAdmins() {
    static std::vector<UsersTable::UserRow> admins_vector;

    if (!isAdminsVectorUpdated) {
        admins_vector.clear();

        const auto return_str = fmt::format("SELECT u.* "
                                            "FROM {} u "
                                            "INNER JOIN {} a ON u.user_id = a.user_id;",
                                            UsersTable::TABLE_NAME, AdminsTable::TABLE_NAME);
        SQLite::Statement query(mDbHandler, return_str);
        while (query.executeStep()) {
            admins_vector.push_back(getUserRowFromStatement(query));
        }

        isAdminsVectorUpdated = true;
    }

    return admins_vector;
}

bool DatabaseManager::addTattooArtist(const UsersTable::UserRow& user_row) {
    TattooArtistsTable::TattooArtistRow tattoo_artist_row = {.user_id = user_row.id.value()};
    try {
        const bool is_okay = mDbHandler.exec(
            TattooArtistsTable::formInsertRowQuery(tattoo_artist_row));
        if (is_okay) {
            isTattooArtistsVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::vector<UsersTable::UserRow> DatabaseManager::getTattooArtists() {
    static std::vector<UsersTable::UserRow> tattoo_artists_vector;

    if (!isTattooArtistsVectorUpdated) {
        tattoo_artists_vector.clear();

        const auto return_str = fmt::format("SELECT u.* "
                                            "FROM {} u "
                                            "INNER JOIN {} a ON u.user_id = a.user_id;",
                                            UsersTable::TABLE_NAME, TattooArtistsTable::TABLE_NAME);
        SQLite::Statement query(mDbHandler, return_str);
        while (query.executeStep()) {
            tattoo_artists_vector.push_back(getUserRowFromStatement(query));
        }

        isTattooArtistsVectorUpdated = true;
    }

    return tattoo_artists_vector;
}

bool DatabaseManager::addMaterialAlarmUser(const UsersTable::UserRow& user_row) {
    MaterialAlarmUsersTable::MaterialAlarmUserRow row = {.user_id = user_row.id.value()};
    try {
        const bool is_okay = mDbHandler.exec(MaterialAlarmUsersTable::formInsertRowQuery(row));
        if (is_okay) {
            isMaterialAlarmUsersVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::deleteMaterialAlarmUserByUserId(std::int64_t user_id) {
    try {
        const bool is_okay = mDbHandler.exec(MaterialAlarmUsersTable::formDeleteRowQuery(user_id));
        if (is_okay) {
            isMaterialAlarmUsersVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::vector<UsersTable::UserRow> DatabaseManager::getMaterialAlarmUsers() {
    static std::vector<UsersTable::UserRow> material_alarm_users_vector;

    if (!isMaterialAlarmUsersVectorUpdated) {
        material_alarm_users_vector.clear();

        const auto return_str = fmt::format("SELECT u.* "
                                            "FROM {} u "
                                            "INNER JOIN {} m ON u.user_id = m.user_id;",
                                            UsersTable::TABLE_NAME,
                                            MaterialAlarmUsersTable::TABLE_NAME);
        SQLite::Statement query(mDbHandler, return_str);
        while (query.executeStep()) {
            material_alarm_users_vector.push_back(getUserRowFromStatement(query));
        }

        isMaterialAlarmUsersVectorUpdated = true;
    }

    return material_alarm_users_vector;
}

bool DatabaseManager::addMaterialCriticalAmount(const MaterialsTable::MaterialRow& material_row,
                                                std::int64_t critical_amount) {
    MaterialCriticalAmountTable::MaterialCriticalAmountRow row = {
        .material_id = material_row.id.value(), .critical_amount = critical_amount};
    try {
        const bool is_okay = mDbHandler.exec(MaterialCriticalAmountTable::formInsertRowQuery(row));
        if (is_okay) {
            isMaterialCriticalAmountVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::updateMaterialCriticalAmountById(
    std::int64_t id, const MaterialCriticalAmountTable::MaterialCriticalAmountRow& material_row) {
    try {
        const bool is_okay = mDbHandler.exec(
            MaterialCriticalAmountTable::formUpdateRowQuery(id, material_row));
        if (is_okay) {
            isMaterialCriticalAmountVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::deleteMaterialCriticalAmount(std::int64_t id) {
    try {
        const bool is_okay = mDbHandler.exec(MaterialCriticalAmountTable::formDeleteRowQuery(id));
        if (is_okay) {
            isMaterialCriticalAmountVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::deleteMaterialCriticalAmountByMaterialId(std::int64_t material_id) {
    const auto critical_amount_materials = getMaterialCriticalAmount();
    const auto found_material            = std::find_if(critical_amount_materials.begin(),
                                                        critical_amount_materials.end(),
                                                        [material_id](const auto& material_row) {
                                                 return (material_row.material_id == material_id);
                                             });
    if (found_material != critical_amount_materials.end()) {
        return deleteMaterialCriticalAmount(found_material->id.value());
    }
    return false;
}

std::vector<MaterialCriticalAmountTable::MaterialCriticalAmountRow>
DatabaseManager::getMaterialCriticalAmount() {
    static std::vector<MaterialCriticalAmountTable::MaterialCriticalAmountRow> materials_vector;

    if (!isMaterialCriticalAmountVectorUpdated) {
        materials_vector.clear();

        SQLite::Statement query(mDbHandler, MaterialCriticalAmountTable::formSelectRowQuery());
        while (query.executeStep()) {
            materials_vector.push_back(getMaterialCriticalAmountRowFromStatement(query));
        }

        isMaterialCriticalAmountVectorUpdated = true;
    }

    return materials_vector;
}

std::optional<MaterialCriticalAmountTable::MaterialCriticalAmountRow>
DatabaseManager::getMaterialCriticalAmountByMaterialId(std::int64_t material_id) {
    const auto critical_materials      = getMaterialCriticalAmount();
    const auto found_critical_material = std::find_if(
        critical_materials.begin(), critical_materials.end(),
        [material_id](const auto& material_row) {
            return (material_id == material_row.material_id);
        });
    if (found_critical_material != critical_materials.end()) {
        return *found_critical_material;
    }
    return {};
}

bool DatabaseManager::addMaterial(const MaterialsTable::MaterialRow& material_row) {
    try {
        const bool is_okay = mDbHandler.exec(MaterialsTable::formInsertRowQuery(material_row));
        if (is_okay) {
            isMaterialsVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::updateMaterialCountById(std::int64_t id,
                                              const MaterialsTable::MaterialRow& material_row) {
    try {
        const bool is_okay = mDbHandler.exec(MaterialsTable::formUpdateRowQuery(id, material_row));
        if (is_okay) {
            isMaterialsVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

bool DatabaseManager::deleteMaterialById(std::int64_t id) {
    try {
        const bool is_okay = mDbHandler.exec(MaterialsTable::formDeleteRowQuery(id));
        if (is_okay) {
            isMaterialsVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::vector<MaterialsTable::MaterialRow> DatabaseManager::getMaterials() {
    static std::vector<MaterialsTable::MaterialRow> materials_vector;

    if (!isMaterialsVectorUpdated) {
        materials_vector.clear();

        SQLite::Statement query(mDbHandler, MaterialsTable::formSelectRowQuery());
        while (query.executeStep()) {
            materials_vector.push_back(getMaterialRowFromStatement(query));
        }

        isMaterialsVectorUpdated = true;
    }

    return materials_vector;
}

std::optional<MaterialsTable::MaterialRow>
DatabaseManager::getMaterialByName(const std::string& name) {
    const auto materials = getMaterials();
    const auto material  = std::find_if(materials.begin(), materials.end(),
                                        [name](const auto& material_row) {
                                           return (material_row.name == name);
                                       });

    if (material != materials.end()) {
        return *material;
    }

    return {};
}

std::optional<MaterialsTable::MaterialRow> DatabaseManager::getMaterialById(size_t id) {
    const auto materials = getMaterials();
    const auto material  = std::find_if(materials.begin(), materials.end(),
                                        [id](const auto& material_row) {
                                           return (material_row.id == id);
                                       });

    if (material != materials.end()) {
        return *material;
    }

    return {};
}

void DatabaseManager::initUsersTable() {
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

void DatabaseManager::initTattooArtistsTable() {
    try {
        mDbHandler.exec(TattooArtistsTable::formCreateTableQuery());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void DatabaseManager::initMaterialAlarmUsersTable() {
    try {
        mDbHandler.exec(MaterialAlarmUsersTable::formCreateTableQuery());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void DatabaseManager::initMaterialCriticalAmountTable() {
    try {
        mDbHandler.exec(MaterialCriticalAmountTable::formCreateTableQuery());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void DatabaseManager::initSessionsTable() {
    try {
        mDbHandler.exec(SessionsTable::formCreateTableQuery());
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

MaterialCriticalAmountTable::MaterialCriticalAmountRow
DatabaseManager::getMaterialCriticalAmountRowFromStatement(SQLite::Statement& statement) {
    MaterialCriticalAmountTable::MaterialCriticalAmountRow material_critical_amount_row{};

    if (!statement.getColumn(0).isNull()) {
        material_critical_amount_row.id = statement.getColumn(0).getInt64();
    }
    if (!statement.getColumn(1).isNull()) {
        material_critical_amount_row.material_id = statement.getColumn(1).getInt64();
    }
    if (!statement.getColumn(2).isNull()) {
        material_critical_amount_row.critical_amount = statement.getColumn(2).getInt64();
    }

    return material_critical_amount_row;
}

SessionsTable::SessionRow
DatabaseManager::getSessionRowFromStatement(SQLite::Statement& statement) {
    SessionsTable::SessionRow session_row{};
    if (!statement.getColumn(0).isNull()) {
        session_row.id = statement.getColumn(0).getInt64();
    }
    if (!statement.getColumn(1).isNull()) {
        session_row.date_time = statement.getColumn(1).getString();
    }
    if (!statement.getColumn(2).isNull()) {
        session_row.tattoo_artist_id = statement.getColumn(2).getInt64();
    }
    if (!statement.getColumn(3).isNull()) {
        session_row.user_id = statement.getColumn(3).getInt64();
    }

    return session_row;
}

std::vector<MaterialsTable::MaterialRow> DatabaseManager::getCriticalMaterials() {
    const auto materials = getMaterials();
    std::vector<MaterialsTable::MaterialRow> critical_materials;
    for (const auto& material : materials) {
        const auto critical_amount = getMaterialCriticalAmountByMaterialId(material.id.value());
        if (critical_amount) {
            if (material.count <= critical_amount.value().critical_amount) {
                critical_materials.push_back(material);
            }
        }
    }

    return critical_materials;
}

bool DatabaseManager::addSession(const SessionsTable::SessionRow& row) {
    try {
        const bool is_okay = mDbHandler.exec(SessionsTable::formInsertRowQuery(row));
        if (is_okay) {
            isSessionsVectorUpdated = false;
        }
        return is_okay;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return false;
    }
}

std::vector<SessionsTable::SessionRow> DatabaseManager::getSessions() {
    static std::vector<SessionsTable::SessionRow> sessions_vector;

    if (!isSessionsVectorUpdated) {
        sessions_vector.clear();

        SQLite::Statement query(mDbHandler, SessionsTable::formSelectRowQuery());
        while (query.executeStep()) {
            sessions_vector.push_back(getSessionRowFromStatement(query));
        }

        isSessionsVectorUpdated = true;
    }

    return sessions_vector;
}

std::vector<SessionsTable::SessionRow> DatabaseManager::getSessionsInFuture() {
    std::vector<SessionsTable::SessionRow> ret_vec;
    const auto sessions = getSessions();
    const auto now      = std::chrono::system_clock::now();
    std::for_each(sessions.begin(), sessions.end(), [&ret_vec, now](const auto& session) {
        if (DatabaseManagerTools::convertStrToTimePoint(session.date_time) > now) {
            ret_vec.push_back(session);
        }
    });
    return ret_vec;
}
