#include <value/mysql/connection.hpp>
#include <value/mysql/connection_invariants.hpp>
#include <value/mysql/statement.hpp>

#include "connection/impl.hpp"
#include "error.hpp"

#include <mysql.h>
#include <value/mysql/mysql_api.hpp>

#include <deque>
#include <map>
#include <exception>
#include <stdexcept>
#include <set>




namespace value  { namespace mysql {
    
    struct connection::library_initialiser
    {
        library_initialiser()
        {
            mysql_library_init(0, nullptr, nullptr);
        }
        
        ~library_initialiser()
        {
            mysql_library_end();
        }
    };
    
    struct error : std::runtime_error
    {
        error(MYSQL& mysql)
        : std::runtime_error(make_error_string(mysql))
        {
        }
        
        error(MYSQL& mysql, const std::string& context)
        : std::runtime_error(add_context(make_error_string((mysql)),
                                         context))
        {
        }
        
        static std::string make_error_string(MYSQL& mysql)
        {
            return { mysql_error(std::addressof(mysql)) };
        }
        
        static std::string add_context(std::string original_error, const std::string& context)
        {
            original_error += ", context: " + context;
            return original_error;
        }
    };
    
    
    
    
    
    
    
    
    //
    // implement outer class
    //
    
    auto connection::acquire_library() -> library_initialiser&
    {
        static library_initialiser _ {};
        return _;
    }
    
    auto connection::make_impl(connection_parameters params) -> impl_ptr
    {
        acquire_library();
        return std::make_shared<impl>(std::move(params));
    }
    
    connection::connection(const connection_invariants& params)
    : _impl(std::make_shared<impl>(std::move(params)))
    {
        
    }
    
    shared_mysql_ptr connection::mysql() const
    {
        return { _impl, static_cast<MYSQL*>(_impl.get()) };
    }
    
    auto connection::begin_transaction() -> void
    try
    {
        using ::value::mysql::begin_transaction;
        
        begin_transaction(_impl);
    }
    catch(...) {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    auto connection::commit(bool reopen) -> void
    try
    {
        using ::value::mysql::commit;
        commit(_impl, reopen);
    }
    catch(...) {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    auto connection::rollback() -> void
    try
    {
        using ::value::mysql::rollback;
        rollback(_impl);
    }
    catch(...) {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    auto connection::rollback(without_exception_t without) noexcept -> void
    {
        try {
            using ::value::mysql::rollback;
            rollback(_impl);
        }
        catch(...)
        {
            
        }
    }
    


    

    
}}


