#pragma once

/// @file describe the traits of a database engine.
/// A database engine maps high level concepts (tables, query types) to low level operations through
/// a connector
/// The driver may make use of SQL generation
///

namespace value { namespace data {
    
    template<class DatabaseEngine>
    struct database_traits;
        // concepts:
        
        // using ddl_statement_type =
        // using connection_type =
    // database_type =
    
}}