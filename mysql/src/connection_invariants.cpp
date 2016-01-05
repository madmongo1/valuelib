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
        
        std::string client_flags_as_text(unsigned long flags)
        {
            using namespace std;
            ostringstream ss;
            ss << hex << flags;
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
        os << ", flags=" << client_flags_as_text(_client_flag);
        
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
    
}}
