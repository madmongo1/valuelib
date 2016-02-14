#pragma once

namespace value { namespace data {
  
    template<class...Fields>
    struct field_list
    {
        
    };

    struct index_component_tag {};

    template<class Field>
    struct field_reference
    {
        using field_type = Field;
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
    
    template<class FieldList, class PrimaryKeyIndex = index<>, class Indexes = index_list<>>
    struct table_entity
    {
        using tag_type = entity_tag;
        
    };
}}