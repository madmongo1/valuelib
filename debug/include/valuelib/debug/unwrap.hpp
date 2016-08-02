#pragma once
#include <ostream>
#include <exception>
#include <stdexcept>
#include <sstream>
#include "demangle.hpp"


namespace value { namespace debug {
    
    struct nonstandard_exception : std::exception
    {
        const char* what() const noexcept override;
    };
    
    struct no_exception : std::exception
    {
        const char* what() const noexcept override;
    };
    
    namespace detail {
        
        template<class Formatter>
        struct unwrapper
        {
            unwrapper(std::exception_ptr pe, Formatter&& formatter)
            : pe(pe)
            , _format(std::move(formatter))
            {}
            
            void write(std::ostream& os, std::exception_ptr ep, size_t depth = 0) const
            {
                try {
                    std::rethrow_exception(ep);
                }
                catch(const std::exception& e) {
                    write(os, e, depth);
                }
                catch(...) {
                    _format(os, nonstandard_exception(), depth);
                }
            }
            
            void write(std::ostream& os, const std::exception& e, size_t depth = 0) const
            {
                _format(os, e, depth);
                try {
                    std::rethrow_if_nested(e);
                }
                catch(const std::exception& e) {
                    write(os, e, depth + 1);
                }
                catch(...) {
                    _format(os, nonstandard_exception(), depth + 1);
                }
            }
            
            std::ostream& write(std::ostream& os) const
            {
                write(os, pe);
                return os;
            }
            
            const std::exception_ptr pe;
            Formatter _format;
        };
        
        template<class Formatter>
        std::ostream& operator<<(std::ostream& os, const unwrapper<Formatter>& u)
        {
            return u.write(os);
        }

        template<class Formatter>
        std::string to_string(const unwrapper<Formatter>& u)
        {
            std::ostringstream ss;
            u.write(ss);
            return ss.str();
        }
        
        
    }
    
    std::string tidy_exception_name(const std::exception& e);
    
    
    struct indenting_formatter
    {
        void operator()(std::ostream& os, const std::exception& e, size_t depth) const;
    };
    
    struct csv_formatter
    {
        void operator()(std::ostream& os, const std::exception& e, size_t depth) const;
    };
    
    template<typename Formatter = indenting_formatter>
    auto unwrap(const std::exception& e,
                Formatter&& formatter = indenting_formatter())
    {
        return detail::unwrapper<Formatter>(make_exception_ptr(e),
                                            std::forward<Formatter>(formatter));
    }
    
    template<typename Formatter = indenting_formatter>
    auto unwrap(const std::exception_ptr& ep = std::current_exception(),
                Formatter&& formatter = indenting_formatter())
    {
        if (not ep)
        {
            static const no_exception _no_exception {};
            return detail::unwrapper<Formatter>(std::make_exception_ptr(_no_exception),
                                                std::forward<Formatter>(formatter));
        }
        else {
            return detail::unwrapper<Formatter>(ep,
                                                std::forward<Formatter>(formatter));
        }
    }
    
}}