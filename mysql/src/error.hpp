#pragma once
#include <value/mysql/error.hpp>

#include <mysql.h>

namespace value { namespace mysql {

    [[noreturn]]
    inline bool throw_database_error(MYSQL* mysql)
    {
        using std::to_string;
        using namespace std::literals;
        throw database_error("MYSQL Error : "s + to_string(mysql_errno(mysql)) + " : " + mysql_error(mysql));
    }

    [[noreturn]]
    inline bool throw_database_error(MYSQL& mysql)
    {
        throw throw_database_error(std::addressof(mysql));
    }
    
    inline bool throw_statement_error(MYSQL_STMT* stmt)
    {
        using std::to_string;
        using namespace std::literals;

        auto errnum = mysql_stmt_errno(stmt);
        if (errnum) {
            throw statement_error("MYSQL STMT Error: "s
                                  + to_string(errnum)
                                  + " SQLSTATE : " + mysql_stmt_sqlstate(stmt)
                                  + " : " + mysql_stmt_error(stmt));
        }
        return false;
    }

}}