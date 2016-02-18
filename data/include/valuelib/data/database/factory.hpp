#pragma once
#include <entity.hpp>
#include <valuelib/data/entity.hpp>
#include <valuelib/data/database_engine.hpp>

namespace value { namespace data { namespace database {

    
    template<class Concept, class Database>
    const ddl<Database>& ddl();
    
    template<class Concept, class Database>
    struct model_concept<Concept, Database>;

    template<class Concept, class Database>
    std::shared_ptr<Concept> build_concept_model(value::data::connection_t<Database> conn);
    
    template<class Concept, class Database>
    struct concept_factory
    {
        static const entity<Concept, Database>& ddl_entity();
        
    };
    
}}}