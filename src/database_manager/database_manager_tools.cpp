#include "database_manager_tools.h"

#include <felisss_logger/felisss_logger.h>

#include <fmt/core.h>

#include <iomanip>
#include <regex>

using namespace std::chrono;

std::optional<std::chrono::system_clock::time_point>
DatabaseManagerTools::convertStrToTimePoint(const std::string& time_str) {
    try {
        if (!isValidDateTimeFormat(time_str)) {
            SPDLOG_ERROR("\"{}\" is not valid DateTime string", time_str);
            return {};
        }
        std::tm tm{};
        std::istringstream ss(time_str);
        if (time_str.find(":") != std::string::npos) {
            ss >> std::get_time(&tm, "%d/%m/%Y %H:%M");
        } else {
            ss >> std::get_time(&tm, "%d/%m/%Y");
        }
        tm.tm_isdst        = -1;
        std::time_t time_c = std::mktime(&tm);
        if (time_c == -1) {
            throw std::runtime_error("Failed to convert time");
        }
        return std::chrono::system_clock::from_time_t(time_c);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return {};
    }
}

std::optional<std::string> DatabaseManagerTools::convertTimePointToStr(
    const std::chrono::system_clock::time_point& time_point) {
    try {
        const std::time_t tt = std::chrono::system_clock::to_time_t(time_point);
        std::tm* local_tm    = std::localtime(&tt);
        std::stringstream ss;
        ss << std::put_time(local_tm, "%d/%m/%Y %H:%M");
        return ss.str();
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        return {};
    }
}

bool DatabaseManagerTools::isValidDateTimeFormat(const std::string& time_str) {
    // Regular expression to match "DD/MM/YYYY HH:MM"
    std::regex full_dt_format(R"(^\d{2}/\d{2}/\d{4} \d{2}:\d{2}$)");
    // Regular expression to match "DD/MM/YYYY"
    std::regex date_format(R"(^\d{2}/\d{2}/\d{4}$)");

    return (std::regex_match(time_str, full_dt_format) || std::regex_match(time_str, date_format));
}

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

bool DatabaseManagerTools::validateUserInput(const std::string& str) {
    std::regex pattern("^[^'\";%]*$");
    if (std::regex_match(str, pattern)) {
        return true;
    } else {
        return false;
    }
}
