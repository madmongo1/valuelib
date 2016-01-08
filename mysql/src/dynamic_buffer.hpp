#pragma once

#include <value/mysql/optional.hpp>
#include <value/mysql/types.hpp>

#include <mysql.h>
#include <cassert>
#include <memory>

namespace value { namespace mysql {
  
    
    
    struct dynamic_buffer
    {
        static constexpr size_t chunk_size = 32;
        
        using store_block = std::aligned_storage<chunk_size>;
        
        static constexpr size_t block_size = sizeof(store_block);
        
        /// @pre bind->data_type has been set
        dynamic_buffer(MYSQL_BIND* bind);
        
        bool resize_on_error()
        {
            if (is_error) {
                assert(data_length > size());
                auto new_blocks = (data_length + block_size - 1) / block_size;
                _data = std::make_unique<store_block[]>(new_blocks);
                _blocks = new_blocks;
                _bind->buffer = data();
                _bind->buffer_length = size();
                is_error = 0;
                return true;
            }
            return false;
        }
        
        void* data() {
            auto p = _data.get();
            if (not p)
                p = &_static_store;
            return p;
        }
        
        size_t size() const {
            return _blocks * chunk_size;
        }
        
        MYSQL_BIND* _bind;
        
        // where field_data will be received
//        field_data _storage;
        
        store_block _static_store;
        std::unique_ptr<store_block[]> _data;
        size_t _blocks;
        my_bool is_error = 0;
        my_bool is_null = 0;
        unsigned long data_length = 0;
        
        // this is where the final field data goes
        optional<field_data> _field_data;
        
    };

    
}}