#pragma once
#include "column.hpp"
#include "metafunctions.hpp"

namespace value { namespace data {
 
    struct index_component_tag {};

    template<class Column>
    struct column_ref
    {
        using column_definition = Column;
        using tag_type = index_component_tag;
        static constexpr auto column() { return Column(); }
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
    
    template<
    class Identifier,
    class ColumnList,
    class PrimaryKeyIndex = no_primary_key_type,
    class Indexes = index_list<>
    >
    struct table
    {
        // CONCEPT:
        // tag_type::identifier() -> data::immutable::string<>
        using identifier_type = Identifier;
        static constexpr auto identifier() { return identifier_type::identifier(); }
        
        
        
        // identifies the kind of entity
        using object_type_tag = entity_tag;
        
        // is a column_list<...>
        using column_list_type = ColumnList;
        static constexpr auto columns() { return column_list_type(); }
        
//        using primary_key_type = PrimaryKeyIndex;
        static constexpr auto primary_key(){ return value::data::primary_key_type<PrimaryKeyIndex>(); }
        
        using index_list_type = Indexes;
    };
    /// override metafunctions for a table
    namespace metafunction { namespace impl {
        template<class Identifier, class ColumnList, class PrimaryKeys, class Indexes>
        struct entity_dependencies< table<Identifier, ColumnList, PrimaryKeys, Indexes> >
        {
            // a table has no dependencies. It is the base item
            using result = std::tuple<>;
        };
    }}
}}