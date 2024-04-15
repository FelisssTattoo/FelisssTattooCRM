#pragma once

#include "database_manager_types.h"

#include <chrono>
#include <string>

class DatabaseManagerTools {
public:
    static std::string sqlTypeToStr(SqlType sql_type);
    static std::string formCreateTableQuery(const std::string& table_name,
                                            std::vector<ColumnType> columns);
    static std::chrono::system_clock::time_point convertStrToTimePoint(const std::string& time_str);
};
