#pragma once

#include <string>

namespace value { namespace mysql {

    struct execution_result;
    struct connection;
    
    struct result_set;
    
    struct sequential_result_set
    {
        struct iterator
        {
            
        };
        
        struct const_iterator
        {
            
        };
    };

	struct statement
	{
        statement(connection con, const std::string& sql);

        struct sequential {};
        struct random_access {};

        statement& execute();
        size_t affected_rows() const;


        execution_result results();
        
        std::ostream& write(std::ostream&) const;

        struct impl;
        using impl_ptr = std::shared_ptr<impl>;
        impl_ptr _impl_ptr;
	};
    
    std::ostream& operator<<(std::ostream& os, const statement& stmt);
    
    statement make_statement(connection conn, const std::string& sql);
    

    
}}