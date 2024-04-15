#include "database_manager.h"

#include "customers_table.h"

DatabaseManager::DatabaseManager(std::string_view db_pathname) :
    mDbPathname(db_pathname),
    mDbHandler(mDbPathname, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    initCustomersTable();
}

void DatabaseManager::addCustomer(const CustomersTable::CustomersRow& customer_row) {
    mDbHandler.tryExec(CustomersTable::formInsertRowQuery(customer_row));
}

void DatabaseManager::initCustomersTable() {
    mDbHandler.tryExec(CustomersTable::formCreateTableQuery());
}
