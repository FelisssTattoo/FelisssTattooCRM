#pragma once

#include <string_view>
#include <vector>

enum class SqlType {
    INTEGER,
    TEXT,
    BLOB,
    REAL
};

struct ColumnType {
    std::string_view column_name;
    SqlType sql_type;
    std::vector<std::string_view> constraints;
};
