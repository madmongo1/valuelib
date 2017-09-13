#pragma once

#include <boost/any.hpp>
#include <iostream>
#include <memory>
#include <typeindex>
#include <functional>

namespace value { namespace debug {

    struct any_printer
    {
        template<class T>
        using print_function = std::function<void (std::ostream& os, const T& val)>;
        
        struct concept
        {
            virtual void print(std::ostream&, const boost::any& a) const = 0;
            virtual ~concept() = default;
        };
        
        template<class T> struct model final : concept {
            model(print_function<T>&& f) : _f(std::move(f)) {}
            void print(std::ostream&os, const boost::any& a) const override
            {
                _f(os, boost::any_cast<const T&>(a));
            }
        private:
            print_function<T> _f;
        };
        
        struct impl;
        static impl& get_impl();
        
        static std::ostream& print(std::ostream& os, const boost::any& a);
        static bool add_printer_impl(std::type_index ti, std::unique_ptr<concept>&& pc);
        
        template<class T>
        static bool add_printer(print_function<T> func)
        {
            return add_printer_impl(typeid(T), std::make_unique<model<T>>(std::move(func)));
        }
        
        template<class T, class Function>
        static bool add_printer(Function&& f)
        {
            return add_printer<T>(print_function<T>(std::forward<Function>(f)));
        }
        
    };

}}

namespace boost {
    inline std::ostream& operator<<(std::ostream& os, const boost::any& a)
    {
        return value::debug::any_printer::print(os, a);
    }
}
