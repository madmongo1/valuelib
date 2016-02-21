#pragma once
#include <ostream>
#include <exception>
#include <stdexcept>
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
            unwrapper(const std::exception& e, Formatter&& formatter)
            : _e(e)
            , _format(std::move(formatter))
            {}
            
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
            
            void write(std::ostream& os) const
            {
                write(os, _e);
            }
            
            const std::exception& _e;
            Formatter _format;
        };
        
        template<class Formatter>
        std::ostream& operator<<(std::ostream& os, const unwrapper<Formatter>& u)
        {
            u.write(os);
            return os;
        }
    }
    
    std::string tidy_exception_name(const std::exception& e);
    
    
    struct indenting_formatter
    {
        void operator()(std::ostream& os, const std::exception& e, size_t depth) const;
    };
    
    template<typename Formatter = indenting_formatter>
    auto unwrap(const std::exception& e,
                Formatter&& formatter = indenting_formatter())
    {
        return detail::unwrapper<Formatter>(e,
                                            std::forward<Formatter>(formatter));
    }
    
    template<typename Formatter = indenting_formatter>
    auto unwrap(const std::exception_ptr& ep = std::current_exception(),
                Formatter&& formatter = indenting_formatter())
    {
        try {
            static const no_exception _no_exception {};
            std::rethrow_exception(ep);
            return detail::unwrapper<Formatter>(_no_exception,
                                                std::forward<Formatter>(formatter));
        }
        catch(const std::exception& e)
        {
            return detail::unwrapper<Formatter>(e,
                                                std::forward<Formatter>(formatter));
        }
        catch(...)
        {
            static const nonstandard_exception _nonstandard {};
            return unwrap(_nonstandard, std::forward<Formatter>(formatter));
        }
    }
    
}}