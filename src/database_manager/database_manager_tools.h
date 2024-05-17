#pragma once

#include "database_manager_types.h"

#include <chrono>
#include <optional>
#include <string>

class DatabaseManagerTools {
public:
    static std::optional<std::chrono::system_clock::time_point>
    convertStrToTimePoint(const std::string& time_str);
    static std::optional<std::string>
    convertTimePointToStr(const std::chrono::system_clock::time_point& time_point);
    static bool isValidDateTimeFormat(const std::string& time_str);
    static std::tm convertTimePointToTm(const std::chrono::system_clock::time_point& time_point);

    static std::string sqlTypeToStr(SqlType sql_type);
    static std::string formCreateTableQuery(const std::string& table_name,
                                            std::vector<ColumnType> columns);
    static bool validateUserInput(const std::string& str);
};
