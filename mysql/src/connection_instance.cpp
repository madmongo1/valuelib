#include "connection_instance.hpp"
#include "error.hpp"

#include <cassert>

namespace value { namespace mysql {
    
    namespace {
        std::string make_connection_context(const host& host, const port& port, const username& user)
        {
            using namespace std;
            auto result = "connecting: "s;
            if (user.empty()) {
                result += "{none}";
            } else {
                result += user;
            }
            result += '@';
            result += host;
            result += ':';
            result += port;
            return result;
        }
        
        std::string make_connection_context(const connection_invariants& invariants)
        {
            using namespace std;
            return "using invariants"s;
        }
        
    }
    
    connection_instance::connection_instance(const host& host,
                                             port port,
                                             const username& user,
                                             const password& password,
                                             const schema& default_schema)
    try
    {
        mysql_real_connect(std::addressof(_mysql),
                           host.c_str(), user.c_str(),
                           password.c_str(),
                           default_schema.c_str(),
                           port,
                           nullptr,
                           CLIENT_MULTI_STATEMENTS)
        or throw_database_error(_mysql);
        _need_close = true;
    }
    catch(...) {
        std::throw_with_nested(connection_error(make_connection_context(host, port, user)));
    }
    
    connection_instance::connection_instance(const connection_invariants& invariants)
    try
    {
        throw std::logic_error("not implemented");
    }
    catch(...) {
        std::throw_with_nested(connection_error(make_connection_context(invariants)));
    }

    connection_instance::~connection_instance() noexcept
    {
        close();
    }

    auto connection_instance::close() noexcept -> void
    {
        if (_need_close) {
            _need_close = false;
            mysql_close(std::addressof(_mysql));
        }
    }
    
    auto connection_instance::pings_ok() noexcept -> bool
    {
        // precondition : the connection must be open
        assert(_need_close);
        
        return 0 == mysql_ping(std::addressof(_mysql));
    }

    
}}