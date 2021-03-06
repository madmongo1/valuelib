#pragma once
#include "column.hpp"
#include "metafunctions.hpp"
#include <utility>
#include "type_traits.hpp"

namespace value { namespace data {
 
    struct index_component_tag {};

    template<class Column>
    struct column_ref
    {
        using column_definition = Column;
        using tag_type = index_component_tag;
        static constexpr auto column() { return Column(); }
    };
    
    template<class Table, class Column>
    struct table_column_ref
    {
        using column_definition = Column;
        using table_definition = Table;
        using tag_type = index_component_tag;
        static constexpr auto column() { return column_definition(); }
        static constexpr auto table() { return table_definition(); }
    };
    
    template<class Expression>
    struct ascending : Expression
    {
        static constexpr bool descending_index = false;
    };
    
    template<class Expression>
    struct descending : Expression
    {
        static constexpr bool descending_index = true;
    };
    
    template<class...Expressions>
    struct index
    {
        static constexpr bool empty() { return sizeof...(Expressions) == 0; }
        static constexpr auto as_tuple() { return std::make_tuple(Expressions()...); }
    };
    template<class...Expressions>
    static constexpr auto make_index(Expressions...expressions)
    {
        return std::make_tuple(expressions...);
    }
    
    template<class Name, class...Expressions>
    struct named_index
    {
        
    };
    
    template<class...NamedIndexes>
    struct index_list
    {
        
    };
    
    struct no_primary_key_type {};
    
    template<class Index>
    struct primary_key_type
    {
        static constexpr auto index() { return Index(); }
    };
    
    // a data entity
    struct entity_tag {};
    
    template<class Identifier>
    struct table
    {
        // CONCEPT:
        // tag_type::identifier() -> data::immutable::string<>
        using identifier_type = Identifier;
        static constexpr auto identifier() { return identifier_type::identifier(); }

        static constexpr auto columns() { return std::make_tuple(); }
        static constexpr auto primary_key() {
            return value::data::primary_key_type<value::data::index<>>();
        }
        static constexpr auto indexes() { return std::make_tuple(); }
    };
    
    template<class T, typename = void> struct is_table : std::false_type {};
    template<class Type>
    struct is_table<Type, std::enable_if_t<is_derived_from_template_v<Type, table>>> : std::true_type {};
    template<class T> static constexpr bool is_table_v = is_table<T>::value;

    
    
    
}}