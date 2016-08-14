#pragma once
#include <valuelib/debug/emit_args.hpp>
#include <valuelib/debug/print.hpp>

namespace value { namespace debug {
    
    namespace detail {
        template<class MethodName, class ArgsType>
        struct method_type
        {
            MethodName name;
            ArgsType args;
            
            friend std::ostream& operator<<(std::ostream& os, const method_type& mt)
            {
                return os << mt.name << '(' << mt.args << ')';
            }
        };
        
        template<class ClassName, class MethodName, class ArgsType>
        struct class_method_type
        {
            ClassName classname;
            method_type<MethodName, ArgsType> method;
            
            friend std::ostream& operator<<(std::ostream& os, const class_method_type& cmt)
            {
                return os << cmt.classname << "::" << cmt.method;
            }
        };
    }
    template<class MethodName, class...Args>
    auto make_method(MethodName&& name, Args&&...args)
    {
        using name_type = std::decay_t<MethodName>;
        using args_type = detail::arg_emitter<std::decay_t<Args>...>;
        return detail::method_type<name_type, args_type> {
            std::forward<MethodName>(name),
            args_type { std::forward<Args>(args)... }
        };
    }
    
    template<class ClassName, class MethodName, class...Args>
    auto class_method(ClassName&& classname, MethodName&& name, Args&&...args)
    {
        using class_type = std::decay_t<ClassName>;
        using name_type = std::decay_t<MethodName>;
        using args_type = detail::arg_emitter<std::decay_t<Args>...>;
        return detail::class_method_type<class_type, name_type, args_type>
        {
            std::forward<ClassName>(classname),
            detail::method_type<name_type, args_type>
            {
                std::forward<MethodName>(name),
                args_type { std::forward<Args>(args)... }
            }
        };
    }
    
    
    
    template<class MethodName, class...Args>
    struct printer<detail::method_type<MethodName, Args...>>
    {
        std::ostream& operator()(std::ostream& os, detail::method_type<MethodName, Args...> const& r) const
        {
            return os << r;
        }
    };
    
    template<class ClassName, class MethodName, class...Args>
    struct printer<detail::class_method_type<ClassName, MethodName, Args...>>
    {
        std::ostream& operator()(std::ostream& os, detail::class_method_type<ClassName, MethodName, Args...> const& r) const
        {
            return os << r;
        }
    };
    
    
}}