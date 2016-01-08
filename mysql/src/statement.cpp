#include <value/mysql/execution_result.hpp>
#include <value/mysql/statement.hpp>
#include <value/mysql/transaction.hpp>

#include "error.hpp"
#include "statement/impl.hpp"

#include <mysql.h>

#include <type_traits>
#include <iostream>
#include <vector>

namespace value { namespace mysql {
    
    struct local_argument
    {
        
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