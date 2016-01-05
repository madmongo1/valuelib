#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/connection_invariants.hpp>

#include <string>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <typeinfo>

namespace value  { namespace mysql {
    
    
    
    struct results {
        
    };
    
    struct statement {
        
        template<class Arg> statement& set_param(size_t index, Arg&& arg);
        template<class...Args> statement& set_params(size_t index, Args&&...args);
        
    };
    
    struct transaction
    {
        struct statement statement();
    };
    
    
    
    
    
    struct connection
    {
        connection(const connection_invariants& params);

        
        struct transaction transaction();
        
    private:
        struct impl;
        using impl_ptr = impl*;
        static auto get_impl(const connection_invariants& params) -> impl_ptr;
        impl_ptr _impl;
    };
    
}}

