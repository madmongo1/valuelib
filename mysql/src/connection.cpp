#include <value/mysql/connection.hpp>
#include <value/mysql/connection_invariants.hpp>

#include <mysql.h>
#include <deque>
#include <map>
#include <exception>
#include <stdexcept>
#include <set>

#include "error.hpp"
#include "connection_instance.hpp"
#include "connection_pool.hpp"


namespace value  { namespace mysql {
    
    struct library_initialiser
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
    
    
    
    using physical_connection_pool = std::deque<connection_instance>;
    using physical_connection_map = std::map<connection_invariants, physical_connection_pool>;
    
    
    
    struct connection::impl
    {
        impl(const connection_invariants& params)
        : _params(params)
        {}
        
        const connection_invariants& params() const {
            return _params;
        }
        
    private:
        connection_invariants _params;
        connection_pool _connection_pool;
    };
    
    
    
    //
    // implement outer class
    //
    
    connection::connection(const connection_invariants& params)
    : _impl(acquire_impl(params))
    {
        
    }

    auto connection::acquire_impl(const connection_invariants& params) -> impl_ptr
    {
        using namespace std;
        
        struct params_less {
            bool operator()(const impl& l, const impl& r) const {
                return l.params() < r.params();
            }
            bool operator()(const impl& l, const connection_invariants& r) const {
                return l.params() < r;
            }
            bool operator()(const connection_invariants& l, const impl& r) const {
                return l < r.params();
            }
            bool operator()(const connection_invariants& l, const connection_invariants& r) const {
                return l < r;
            }
        };
        
        static library_initialiser lib_init;
        static mutex cache_mutex;
        using impl_cache_type = map<connection_invariants, shared_ptr<impl>>;
        static impl_cache_type impl_cache;
        
        lock_guard<mutex> lock(cache_mutex);
        
        auto it = impl_cache.find(params);
        if (it == impl_cache.end()) {
            it = impl_cache.emplace_hint(it, params, make_shared<impl>(params));
        }
        
        return std::addressof(*(it->second));
        
    }
    
    

    
}}


