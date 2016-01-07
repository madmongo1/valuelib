#pragma once

#include "dynamic_buffer.hpp"
#include "mysql_api.hpp"

#include <memory>
#include <vector>

namespace value { namespace mysql {
    
    
    struct result_binder
    {
        using bind_array = MYSQL_BIND[];
        using bind_ptr = std::unique_ptr<bind_array>;
        
        result_binder(MYSQL_STMT* stmt);
        
        bool fetch_next_row();
        
    private:
        void refetch_short_columns();
        
        MYSQL_STMT * _statement;
        result_ptr _metadata_ptr;
        size_t _columns;
        bind_ptr _binds;
        std::vector<dynamic_buffer> _buffers;
        
    };
  
}}