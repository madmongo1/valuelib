//
//  insert.hpp
//  ProjectX
//
//  Created by Richard Hodges on 24/02/2016.
//
//
#pragma once

namespace value { namespace data { namespace sql { namespace mysql {
    
    struct placeholder
    {
        template<class T>
        constexpr auto operator()(T) const {
            return immutable::string("?");
        }
    };
    
    struct backticked_identifier
    {
        template<class Column, std::enable_if_t<is_column_v<Column>>* = nullptr>
        constexpr auto operator()(Column column) const {
            return backtick(column.identifier());
        }
    };
    
    template<class Table, std::enable_if_t<is_table_v<Table>>* = nullptr>
    constexpr auto sql_insert(dialect dialect, Table table)
    {
        using namespace value::immutable;
        
        constexpr auto sql = string("INSERT INTO ") +
        backtick(table.identifier()) +
        " (" +
        join(", ", map(backticked_identifier(), table.columns())) +
        ") VALUES (" +
        join(",", map(placeholder(), table.columns())) + ")";
        return sql;
        
    }
    
}}}}