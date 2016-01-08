#include "impl.hpp"

#include <iostream>

namespace value { namespace mysql {

    auto
    statement::impl::
    build_statement(const shared_mysql_ptr& mysql) -> statement_ptr
    try
    {
        auto ptr = mysql_stmt_init(mysql.get());
        if (!ptr) {
            throw_error(mysql);
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
    
    
    auto
    statement::impl::write(std::ostream& os) const -> std::ostream&
    {
        os << "{";
        os << " bind_params=" << _bind_params.get();
        os << ", statment=" << _stmt.get();
        os << ", parameter_count=" << _parameter_count;
        os << ", metadata" << _meta_data.get();
        return os << " }";
    }


}}
