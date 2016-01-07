#pragma once

#include <mysql.h>
#include <memory>
#include <stdexcept>
#include <exception>

#include "error.hpp"

namespace value { namespace mysql {
    
    
    struct auto_free
    {
        void operator()(MYSQL_RES* p) const noexcept {
            if (p)
                mysql_free_result(p);
        }
    };
    
    using result_ptr = std::unique_ptr<MYSQL_RES, auto_free>;
    
    inline result_ptr fetch_metadata(MYSQL_STMT* stmt)
    try
    {
        auto result = mysql_stmt_result_metadata(stmt);
        if (not result)
            throw_statement_error(stmt);
            
            // warning: if we get here, and result is null, the statement legitimately does not have a result
            return result_ptr(result);
            }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    inline void bind_result(MYSQL_STMT* stmt, MYSQL_BIND* bind)
    try
    {
        if (mysql_stmt_bind_result(stmt, bind))
            throw_statement_error(stmt);
            }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    enum class fetch_result
    {
        success,
        finished,
        truncated
    };
    
    inline fetch_result fetch_next(MYSQL_STMT* stmt)
    try
    {
        auto result = mysql_stmt_fetch(stmt);
        switch(result) {
            case 0:
                return fetch_result::success;
                
            default:
                throw_statement_error(stmt);
                
            case MYSQL_NO_DATA:
                return fetch_result::finished;
                
            case MYSQL_DATA_TRUNCATED:
                return fetch_result::truncated;
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    inline void fetch_column(MYSQL_STMT* stmt, MYSQL_BIND* bind, unsigned int column, unsigned long offset = 0)
    try
    {
        if(mysql_stmt_fetch_column(stmt, bind, column, offset))
        {
            throw_statement_error(stmt);
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    inline void prepare(MYSQL_STMT* stmt, const std::string& sql)
    try
    {
        if(mysql_stmt_prepare(stmt, sql.c_str(), sql.size()))
            throw_statement_error(stmt);
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error("preparing query: "s + sql));
    }
    
    inline void bind_params(MYSQL_STMT* stmt, MYSQL_BIND* bind)
    try
    {
        if(mysql_stmt_bind_param(stmt, bind)) {
            throw_statement_error(stmt);
        }
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    inline void execute(MYSQL_STMT* stmt)
    try
    {
        if(mysql_stmt_execute(stmt)) {
            throw_statement_error(stmt);
        }
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    
}}