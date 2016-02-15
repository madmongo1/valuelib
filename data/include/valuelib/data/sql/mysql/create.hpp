#pragma once

#include <valuelib/data/entity.hpp>
#include <valuelib/immutable/string.hpp>
#include "utility.hpp"
#include <tuple>

namespace value { namespace data { namespace sql { namespace mysql {
    
    // forward declaration
    constexpr auto to_sql();
    template<class Function, class...Ts> constexpr auto map(Function func, std::tuple<Ts...> t);

    
    // nullable
    constexpr auto to_sql(value::data::nullable)
    {
        return value::immutable::string("NULL");
    }
    
    constexpr auto to_sql(value::data::not_null)
    {
        return value::immutable::string("NOT NULL");
    }
    
    // default values
    
    constexpr auto to_sql(value::data::no_default, ...)
    {
        return value::immutable::string("");
    }
    
    constexpr auto to_sql(value::data::current_timestamp, ...)
    {
        return value::immutable::string(" DEFAULT CURRENT_TIMESTAMP");
    }

    template<class NativeType, class Nullable>
    constexpr auto to_sql(value::data::default_from_native,
                          value::data::text_set_storage<NativeType, Nullable> text_set)
    {
        constexpr auto native_type = text_set.get_native_type();
        constexpr auto default_value = static_cast<std::size_t>(native_type.default_value());
        constexpr auto str = std::get<default_value>(native_type.strings());
        return value::immutable::string(" DEFAULT ") + immutable::quoted(str);
    }
    
    template<class NativeType, class Nullable>
    constexpr auto to_sql(value::data::uuid_storage<NativeType, Nullable> uuid)
    {
        return
        value::immutable::string(" VARCHAR(36) ") +
        to_sql(uuid.nullable());
    }
    
    template<class NativeType, class Nullable>
    constexpr auto to_sql(value::data::timestamp_storage<NativeType, Nullable> ts)
    {
        return value::immutable::string(" TIMESTAMP ") + to_sql(ts.nullable());
    }
    
    template<class NativeType, class Nullable>
    constexpr auto to_sql(value::data::text_set_storage<NativeType, Nullable> text_set)
    {
        return value::immutable::string(" SET(")
        + value::immutable::to_quoted_list(text_set.strings())
        + ") "
        + to_sql(text_set.nullable());
    }
    
    template<class Identifier, class Storage, class DefaultValue>
    constexpr auto to_sql(value::data::column<Identifier, Storage, DefaultValue> column)
    {
        return
        backtick(column.identifier()) +
        to_sql(column.storage()) +
        to_sql(column.default_value(), column.storage());
    }

    constexpr auto to_sql(value::data::no_primary_key_type)
    {
        return value::immutable::string("");
    }
    
    template<class Column>
    constexpr auto to_sql(value::data::column_ref<Column> column_ref)
    {
        return backtick(column_ref.column().identifier());
    }
    

    namespace functor
    {
        template<class Function, class Tuple, std::size_t...Is>
        constexpr auto map(Function func, Tuple t, std::integer_sequence<std::size_t, Is...>)
        {
            return std::make_tuple(func(std::get<Is>(t))...);
        }
    }
    
    template<class Function, class...Ts>
    constexpr auto map(Function func, std::tuple<Ts...> t)
    {
        return functor::map(func, t, std::index_sequence_for<Ts...>());
    }


    namespace functor {
        struct to_sql
        {
            template<class T> constexpr auto operator()(T t) const { return value::data::sql::mysql::to_sql(t); }
        };
    }
    
    constexpr auto to_sql()
    {
        return functor::to_sql();
    }

    template<class...Expressions>
    constexpr auto to_sql(value::data::index<Expressions...> idx)
    {
        return value::immutable::join(", ", map(to_sql(), idx.as_tuple()));
    }
    
    template<class Index>
    constexpr auto to_sql(value::data::primary_key_type<Index> primary_key)
    {
        return value::immutable::string(",\nPRIMARY KEY (")
        + to_sql(primary_key.index())
        + ")";
    }
    


    template<class Identifier, class ColumnList, class PrimaryKeyIndex, class Indexes>
    constexpr auto sql_create(value::data::table<Identifier, ColumnList, PrimaryKeyIndex, Indexes> table)
    {
        constexpr auto sql = immutable::string("CREATE TABLE IF NOT EXISTS ") +
        backtick(table.identifier()) + "(\n"
        + value::immutable::join(",\n", map(to_sql(), table.columns().as_tuple()))
        + to_sql(table.primary_key())
        + "\n) ENGINE=InnoDB DEFAULT CHARSET=utf8";
        return sql;

    }



}}}}