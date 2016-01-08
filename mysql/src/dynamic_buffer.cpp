#include "dynamic_buffer.hpp"
#include <value/mysql/types.hpp>

namespace value { namespace mysql  {
  
    /*
    field_data select_storage(MYSQL_BIND * bind, MYSQL_FIELD* field_def)
    {
        switch(bind->buffer_type)
        {
            case enum_field_types::MYSQL_TYPE_BIT:
                return std::string();
            case enum_field_types::MYSQL_TYPE_BLOB:
                if(field_def->charsetnr == 63)
                    return std::vector<uint8_t>(4096);
                else
                    return std::string(20, ' ');
            case enum_field_types::MYSQL_TYPE_DATE:
            case enum_field_types::MYSQL_TYPE_DATETIME:
            case enum_field_types::MYSQL_TYPE_DATETIME2:
            case enum_field_types::MYSQL_TYPE_DECIMAL:
                return std::string(20, ' ');

            case enum_field_types::MYSQL_TYPE_DOUBLE:
                return double();
                
            case enum_field_types::MYSQL_TYPE_ENUM:
                return std::string(20, ' ');
                
            case enum_field_types::MYSQL_TYPE_FLOAT:
                return float();
                
            case enum_field_types::MYSQL_TYPE_GEOMETRY:
                throw std::logic_error("MYSQL_TYPE_GEOMETRY not supported");
                
            case enum_field_types::MYSQL_TYPE_INT24:
                return int();
                
                
        }
        return std::string(20, ' ');
    }
     */

    /// @pre bind->data_type has been set
    dynamic_buffer::dynamic_buffer(MYSQL_BIND* bind)
    : _bind(bind)
//    , _storage(select_storage(bind))
    , _data(nullptr)
    , _blocks(1)
    , is_error(0)
    , is_null(0)
    , data_length(0)
    {
        bind->buffer = data();
        bind->buffer_length = size();
        bind->error = &is_error;
        bind->is_null = &is_null;
        bind->length = &data_length;
    }

}}