#include <value/mysql/connection_invariants.hpp>

#include <iomanip>
#include <sstream>

#include <mysql.h>

namespace value { namespace mysql {
    
    
    namespace {
        template<class Tag>
        const char* ptr_or_null(const boost::optional<tagged_value<std::string, Tag>>& opt)
        {
            if (opt) {
                return opt.get().c_str();
            }
            else {
                return nullptr;
            }
        }
        
        template<class Tag>
        auto for_print(const boost::optional<tagged_value<std::string, Tag>>& opt)
        {
            if (opt) {
                return opt.get();
            }
            else {
                static const tagged_value<std::string, Tag> tag("{null}");
                return tag;
            }
        }
        
        auto for_print(const boost::optional<password>& opt)
        {
            if (opt) {
                static const password stars("******");
                return stars;
            }
            else {
                static const password tag("{null}");
                return tag;
            }
        }
        
        std::string for_print(const client_options& options)
        {
            using namespace std;
            ostringstream ss;
            ss << hex << options;
            return ss.str();
        }

    }
    
    std::ostream& connection_invariants::safe_print(std::ostream& os) const
    {
        using namespace std;
        os << "{";
        
        os << " host=" << quoted(for_print(_host));
        os << ", user=" << quoted(for_print(_user));
        os << ", passwd=" << quoted(for_print(_pwd));
        os << ", db=" << quoted(for_print(_default_schema));
        os << ", port=" << _port;
        os << ", unix_socket=" << quoted(for_print(_unix_socket));
        os << ", flags=" << for_print(_client_flag);
        
        os << " }";
        return os;
    }

    
    const char* connection_invariants::get_host() const
    {
        return ptr_or_null(_host);
    }
    
    const char* connection_invariants::get_user() const
    {
        return ptr_or_null(_user);
    }

    const char* connection_invariants::get_passwd() const
    {
        return ptr_or_null(_pwd);
    }
    
    const char* connection_invariants::get_db() const
    {
        return ptr_or_null(_default_schema);
    }
    
    unsigned int connection_invariants::get_port() const
    {
        return _port;
    }
    
    const char* connection_invariants::get_unix_socket() const
    {
        return ptr_or_null(_unix_socket);
    }
    
    unsigned long connection_invariants::get_client_flag() const
    {
        return _client_flag;
    }
    
    connection_invariants::client_option_builder::client_option_builder() noexcept
    : _options(CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS)
    {
    }
    
    connection_invariants::client_option_builder::operator client_options() const
    {
        return _options;
    }
    
    std::string to_string(const connection_invariants& ci)
    {
        std::ostringstream ss;
        ss << ci;
        return ss.str();
    }

    
}}
