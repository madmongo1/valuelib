#pragma once
#include <valuelib/data/entity.hpp>
#include <valuelib/immutable/string.hpp>
#include "utility.hpp"

namespace value { namespace data { namespace sql { namespace mysql {
    
    template<class Table, std::enable_if_t< is_table_v<Table> >* = nullptr>
    static constexpr auto sql_drop(dialect, Table table)
    {
        return immutable::string("DROP TABLE IF EXISTS ") + backtick(table.identifier());
    }
    
    template<size_t Length>
    static constexpr auto sql_drop_procedure(dialect, immutable::string_type<Length> procname)
    {
        return immutable::string("DROP PROCEDURE IF EXISTS ") + backtick(procname);
    }
    
}}}};