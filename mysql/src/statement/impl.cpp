#include "impl.hpp"

#include "connection_instance.hpp"
#include <iostream>

namespace value { namespace mysql {

    auto
    statement::impl::
    build_statement(const struct transaction& trans) -> statement_ptr
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
