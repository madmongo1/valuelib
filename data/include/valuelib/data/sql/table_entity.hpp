#pragma once
#include <valuelib/data/sql/dialect.hpp>
#include <valuelib/data/entity.hpp>
#include <valuelib/data/database_engine.hpp>

namespace value { namespace data { namespace sql {
   
    template<class Database, class Table, std::enable_if_t< value::data::is_table_v<Table> >* = nullptr>
    const value::data::database::entity<Database>& make_entity(Table table)
    {
        using database_type = Database;
        using database_traits = value::data::database_traits<database_type>;
        using sql_dialect = typename database_traits::sql_dialect;
        using statement_type = typename database_traits::ddl_statement_type;
        
        static struct final : value::data::database::entity<database_type>
        {
            using inherited = value::data::database::entity<database_type>;
            using dependency_list = typename inherited::dependency_list_type;
            
            void create(statement_type& stmt) const override
            {
                static constexpr auto sql = sql_create(sql_dialect(), Table());
                stmt.execute(sql.c_str());
            }
            
            void drop(statement_type& stmt) const override
            {
                constexpr auto sql = sql_drop(sql_dialect(), Table());
                stmt.execute(sql.c_str());
            }
            
            dependency_list const & dependencies() const override
            {
                static dependency_list _ {};
                return _;
            }
            
        } _entity;
        return _entity;
    }

}}}