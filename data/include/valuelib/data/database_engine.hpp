#pragma once

/// @file describe the traits of a database engine.
/// A database engine maps high level concepts (tables, query types) to low level operations through
/// a connector
/// The driver may make use of SQL generation
///

namespace value { namespace data {
    
    template<class DatabaseEngine>
    struct database_traits;

    /// carry traits about a connection type (connection types are assumed to be unique amongst database types)
    template<class DatabaseConnection>
    struct connection_traits;
    // concept:
    // database_type = <some database type>
    
    
    
        // concepts:
        
        // using ddl_statement_type =
        // using connection_type =
    // database_type =
    
    template<class Database> using connection_t = typename database_traits<Database>::connection_type;
    template<class Database> using ddl_statement_t = typename database_traits<Database>::ddl_statement_type;
    template<class ConnectionType> using database_t = typename connection_traits<ConnectionType>::database_type;
    
}}