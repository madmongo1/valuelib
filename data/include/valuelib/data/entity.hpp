#pragma once
#include "column.hpp"

namespace value { namespace data {
 
    struct index_component_tag {};

    template<class Column>
    struct column_ref
    {
        using column_definition = Column;
        using tag_type = index_component_tag;
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
        
    };
    
    template<class Name, class...Expressions>
    struct named_index
    {
        
    };
    
    template<class...NamedIndexes>
    struct index_list
    {
        
    };
    
    // a data entity
    struct entity_tag {};
    
    template<class Tag, class ColumnList, class PrimaryKeyIndex = index<>, class Indexes = index_list<>>
    struct table
    {
        // CONCEPT:
        // tag_type::identifier() -> data::immutable::string<>
        using tag_type = Tag;
        
        // identifies the kind of entity
        using object_type_tag = entity_tag;
        
        // is a column_list<...>
        using column_list_type = ColumnList;
        
        using primary_key_type = PrimaryKeyIndex;
        
        using index_list_type = Indexes;
        
    };
}}