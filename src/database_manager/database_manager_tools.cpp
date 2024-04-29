#include "database_manager_tools.h"

#include <fmt/core.h>

#include <iomanip>
#include <regex>

using namespace std::chrono;

std::string DatabaseManagerTools::sqlTypeToStr(SqlType sql_type) {
    switch (sql_type) {
        case SqlType::INTEGER: {
            return "INTEGER";
        }
        case SqlType::TEXT: {
            return "TEXT";
        }
        case SqlType::BLOB: {
            return "BLOB";
        }
        case SqlType::REAL: {
            return "REAL";
        }
    }
    return "";
}

std::string DatabaseManagerTools::formCreateTableQuery(const std::string& table_name,
                                                       std::vector<ColumnType> columns) {
    std::string return_str{};
    if (!columns.empty()) {
        return_str += fmt::format("CREATE TABLE IF NOT EXISTS \"{}\"(", table_name);
        for (size_t i = 0; i < columns.size(); ++i) {
            return_str += fmt::format("\"{}\" {}", columns.at(i).column_name,
                                      sqlTypeToStr(columns.at(i).sql_type));
            for (const auto& constraint : columns.at(i).constraints) {
                return_str += fmt::format(" {}", constraint);
            }
            if (i != (columns.size() - 1)) {
                return_str += ", ";
            }
        }
        return_str += ");";
    }

    return return_str;
}

system_clock::time_point DatabaseManagerTools::convertStrToTimePoint(const std::string& time_str) {
    std::tm tm{};
    std::stringstream ss(time_str);
    ss >> std::get_time(&tm, "%d/%m/%Y");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

bool DatabaseManagerTools::validateUserInput(const std::string& str) {
    std::regex pattern("^[^'\";%]*$");
    if (std::regex_match(str, pattern)) {
        return true;
    } else {
        return false;
    }
}
