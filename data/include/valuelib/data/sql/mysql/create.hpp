#pragma once

#include <valuelib/data/entity.hpp>
#include <valuelib/immutable/string.hpp>
#include "utility.hpp"
#include <valuelib/data/storage.hpp>
#include <valuelib/data/identifier.hpp>
#include <valuelib/data/entity.hpp>
#include <tuple>

namespace value { namespace data { namespace sql { namespace mysql {
    
    // forward declaration
//    constexpr auto to_sql();
//    template<class T, typename = void> constexpr auto to_sql(T);
    
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
    
    template<class LengthLimit>
    struct to_sql_text_field_impl;
    
    template<std::size_t Length>
    struct to_sql_text_field_impl<value::data::limited_length<Length>>
    {
        static constexpr auto apply()
        {
            return immutable::string(" VARCHAR(") +
            immutable::to_string<std::size_t, Length>() +
            immutable::string(") ");
        }
    };
    
    template<>
    struct to_sql_text_field_impl<value::data::unlimited_length>
    {
        static constexpr auto apply()
        {
            return value::immutable::string(" LONGTEXT ");
        }
    };
    
    template<>
    struct to_sql_text_field_impl<value::data::reasonable_length>
    {
        static constexpr auto apply()
        {
            return value::immutable::string(" VARCHAR(128) ");
        }
    };
    
    template<class NativeType, class LengthLimit, class Nullable>
    constexpr auto to_sql(value::data::string_storage<NativeType, LengthLimit, Nullable> storage)
    {
        return to_sql_text_field_impl<LengthLimit>::apply() + to_sql(Nullable());
    }
    
    template<class Column, std::enable_if_t<is_column_v<Column>>* = nullptr>
    constexpr auto to_sql(Column column)
    {
        return
        backtick(column.identifier()) +
        to_sql(column.storage()) +
        to_sql(column.default_value(), column.storage());
    }
    

    template<class Column>
    constexpr auto to_sql(value::data::column_ref<Column> column_ref);
    
    template<class Function, class...Ts> constexpr auto map(Function func, std::tuple<Ts...> t);
    
    namespace functor {
        struct call_to_sql
        {
            template<class T> constexpr auto operator()(T t) const {
                return to_sql(t);
//                return to_sql(t);
            }
        };
    }
    
    constexpr auto call_to_sql()
    {
        return functor::call_to_sql();
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

    

    
    template<class...Expressions, typename std::enable_if_t<sizeof...(Expressions) != 0>* = nullptr>
    constexpr auto primary_key_clause(std::tuple<Expressions...> tuple)
    {
        return
        value::immutable::string(",\nPRIMARY KEY (") +
        value::immutable::join(", ", map(call_to_sql(), tuple)) +
        ")";
    }
    
    template<class...Expressions, typename std::enable_if_t<sizeof...(Expressions) == 0>* = nullptr>
    constexpr auto primary_key_clause(std::tuple<Expressions...> tuple)
    {
        return value::immutable::string("");
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
    



    template<class...Expressions>
    constexpr auto to_sql(value::data::index<Expressions...> idx)
    {
        return value::immutable::join(", ", map(call_to_sql(), idx.as_tuple()));
    }
    
    template<class Index>
    constexpr auto to_sql(value::data::primary_key_type<Index> primary_key)
    {
        return value::immutable::string(",\nPRIMARY KEY (")
        + to_sql(primary_key.index())
        + ")";
    }
    


    
    template<class Table, std::enable_if_t<is_table_v<Table>>* = nullptr>
    constexpr auto sql_create(dialect dialect, Table table)
    {
        constexpr auto sql = immutable::string("CREATE TABLE IF NOT EXISTS ") +
        backtick(table.identifier()) + "(\n"
        + value::immutable::join(",\n", map(call_to_sql(), table.columns()))
        + primary_key_clause(table.primary_key())
        + "\n) ENGINE=InnoDB DEFAULT CHARSET=utf8";
        return sql;
        
    }
    
    template<class Table, std::enable_if_t<is_table_v<Table>>* = nullptr>
    constexpr auto sql_create(Table table)
    {
        return sql_create(dialect(), table);
    }


}}}}