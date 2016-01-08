#pragma once

#include <value/mysql/execution_result.hpp>
#include <value/mysql/mysql_api.hpp>
#include <value/mysql/statement.hpp>
#include <value/mysql/transaction.hpp>

#include "result_binder.hpp"
#include <value/mysql/mysql_api.hpp>


namespace value { namespace mysql {

    struct statement::impl
    {
        using statement_ptr = std::unique_ptr<MYSQL_STMT, my_bool (*)(MYSQL_STMT*)>;
        using bind_array = MYSQL_BIND[];
        using bind_ptr = std::unique_ptr<MYSQL_BIND[]>;
        
        struct free_helper {
            void operator()(MYSQL_RES* p) const {
                if (p) {
                    mysql_free_result(p);
                }
            }
        };
        using metadata_ptr = std::unique_ptr<MYSQL_RES, free_helper>;
        using result_ptr = std::unique_ptr<MYSQL_RES, free_helper>;
        
        static auto build_statement(const shared_mysql_ptr& mysql) -> statement_ptr;
        
        static constexpr size_t count_parameter_markers(const char* from, const char* to)
        {
            size_t count = 0;
            for ( ; from != to ; ++from )
            {
                if (*from == '?')
                    ++count;
            }
            return count;
        }
        
        impl(connection con, const std::string& sql)
        : _connection(std::move(con))
        , _mysql(_connection.mysql())
        , _stmt(build_statement(_mysql))
        , _parameter_count(mysql_stmt_param_count(_stmt.get()))
        {
            
            _meta_data.reset(mysql_stmt_result_metadata(_stmt.get()));
            throw_error(_stmt.get());
            
            if (_parameter_count) {
                _bind_params = std::make_unique<bind_array>(_parameter_count);
                memset(_bind_params.get(), 0, sizeof(std::remove_extent_t<bind_array>) * _parameter_count);
            }
            
            prepare(_stmt.get(), sql);
        }
        
        void execute()
        {
            if (_arguments_bound) {
                bind_params(_stmt.get(), _bind_params.get());
            }
            
            ::value::mysql::execute(_stmt.get());
        }
        
        size_t affected_rows()
        {
            my_ulonglong rows = mysql_stmt_affected_rows(_stmt.get());
            if (rows == (my_ulonglong)~0)
            {
                throw_error(_stmt.get());
            }
            return size_t(rows);
        }
        
        
        
        execution_result results()
        {
            result_binder binder(_stmt.get());
            while(binder.fetch_next_row())
            {
                
            }
            
            
            return {};
        }
        
        auto write(std::ostream& os) const -> std::ostream&;
        
        
    private:
        
        connection _connection;
        shared_mysql_ptr _mysql;
        
        bind_ptr _bind_params = nullptr;
        statement_ptr _stmt;
        size_t _parameter_count = 0;
        metadata_ptr _meta_data = nullptr;
        
        bool _arguments_bound = false;
    };

}}