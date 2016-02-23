#include <valuelib/debug/any_printer.hpp>
#include <unordered_map>
#include <valuelib/debug/demangle.hpp>
#include <mutex>

namespace value { namespace debug {
    
    struct any_printer::impl {
        using map_type = std::unordered_map<std::type_index, std::unique_ptr<concept>>;
        mutable std::mutex _mutex;
        map_type _map;
        
        bool add_printer(std::type_index ti, std::unique_ptr<concept> printer)
        {
            auto lock = std::unique_lock<std::mutex>(_mutex);
            return _map.emplace(ti, std::move(printer)).second;
        }
        
        void print(std::ostream& os, std::type_index ti, const boost::any& a) const
        {
            try {
                auto lock = std::unique_lock<std::mutex>(_mutex);
                const auto& printer = _map.at(ti);
                lock.unlock();
                printer->print(os, a);
            }
            catch(const std::out_of_range&)
            {
                os << "unregistered object of type: " << value::debug::demangle(ti);
            }
        }
    };
    
    auto any_printer::get_impl() -> impl&
    {
        static impl _{};
        return _;
    }
    
    bool any_printer::add_printer_impl(std::type_index ti, std::unique_ptr<concept>&& pc)
    {
        return get_impl().add_printer(ti, std::move(pc));
    }
    
    std::ostream& any_printer::print(std::ostream& os, const boost::any& a)
    {
        if (a.empty())
            return os << "null";
        else {
            auto& impl = get_impl();
            impl.print(os, a.type(), a);
            return os;
        }
    }
    
    
}}