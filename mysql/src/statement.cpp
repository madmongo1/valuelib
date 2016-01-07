#include <value/mysql/execution_result.hpp>
#include <value/mysql/statement.hpp>
#include <value/mysql/transaction.hpp>

#include "error.hpp"
#include "result_binder.hpp"
#include "transaction/impl.hpp"

#include <mysql.h>

#include <type_traits>
#include <iostream>
#include <vector>

namespace value { namespace mysql {
    
    struct local_argument
    {
        
    };
    
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
        
        static auto build_statement(const struct transaction& trans) -> statement_ptr
        try
        {
            auto mysql = trans.connection_instance().mysql();
            auto ptr = mysql_stmt_init(mysql);
            if (!ptr) {
                throw_database_error(mysql);
            }
            
            return {
                ptr,
                &mysql_stmt_close
                };
        }
        catch(...)
        {
            std::throw_with_nested(std::runtime_error("failed to allocate MYSQL statement"));
        }
        
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
        
        impl(transaction trans, const std::string& sql)
        : _trans(std::move(trans))
        , _stmt(build_statement(_trans))
        , _parameter_count(mysql_stmt_param_count(_stmt.get()))
        {
            
            _meta_data.reset(mysql_stmt_result_metadata(_stmt.get()));
            throw_statement_error(_stmt.get());
            
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
                throw_statement_error(_stmt.get());
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
        
        std::ostream& write(std::ostream& os) const {
            os << "{";
            os << " bind_params=" << _bind_params.get();
            os << ", statment=" << _stmt.get();
            os << ", parameter_count=" << _parameter_count;
            os << ", metadata" << _meta_data.get();
            return os << " }";
        }
        
        
    private:

        transaction _trans;
        bind_ptr _bind_params = nullptr;
        statement_ptr _stmt;
        size_t _parameter_count = 0;
        metadata_ptr _meta_data = nullptr;
        
        bool _arguments_bound = false;
    };
    
    statement::statement(transaction trans, const std::string& sql)
    : _impl_ptr { std::make_shared<impl>(std::move(trans), sql) }
    {
    }

    statement make_statement(transaction trans, const std::string& sql)
    {
        return { std::move(trans), sql };
    }
    
    statement& statement::execute()
    {
        _impl_ptr->execute();
        return *this;
    }
    
    size_t statement::affected_rows() const
    {
        return _impl_ptr->affected_rows();
    }
    
    auto statement::results() -> execution_result
    {
        return _impl_ptr->results();
    }
    
    std::ostream& statement::write(std::ostream& os) const
    {
        return _impl_ptr->write(os);
    }

    
    std::ostream& operator<<(std::ostream& os, const statement& stmt)
    {
        return stmt.write(os);
    }

    

}}