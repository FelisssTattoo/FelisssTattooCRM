#pragma once

#include "customers_table.h"

#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseManager {
public:
    explicit DatabaseManager(std::string_view db_pathname);

    void addCustomer(const CustomersTable::CustomersRow& customer_row);

private:
    void initCustomersTable();

private:
    std::string_view mDbPathname;
    SQLite::Database mDbHandler;

    static constexpr std::string_view CUSTOMER_TABLE_NAME = "customers";
};
