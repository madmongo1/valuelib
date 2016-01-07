#include "result_binder.hpp"
#include "mysql_api.hpp"

namespace value { namespace mysql {
  
    
    result_binder::result_binder(MYSQL_STMT* stmt)
    try
    : _statement(stmt)
    , _metadata_ptr(fetch_metadata(_statement))
    , _columns(_metadata_ptr.get() ? mysql_num_fields(_metadata_ptr.get()) : 0)
    , _binds(_columns ? std::make_unique<bind_array>(_columns) : nullptr)
    {
        if (_columns)
        {
            _buffers.reserve(_columns);
            
            for (size_t c = 0 ; c < _columns ; ++c)
            {
                _binds[c] = { 0 };
                _binds[c].buffer_type = _metadata_ptr->fields[c].type;
                _buffers.emplace_back(&_binds[c]);
            }
            
            bind_result(_statement, _binds.get());
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    bool result_binder::fetch_next_row()
    try
    {
        switch(fetch_next(_statement))
        {
            case fetch_result::success:
                return true;
                
            case fetch_result::finished:
                return false;
                
            case fetch_result::truncated:
                refetch_short_columns();
                return true;
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    void result_binder::refetch_short_columns()
    try
    {
        for (size_t c = 0 ; c < _columns ; ++c)
        {
            if (_buffers[c].resize_on_error())
            {
                fetch_column(_statement, std::addressof(_binds[c]), c);
            }
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }



}}