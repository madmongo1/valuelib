#include "valuelib/debug/unwrap.hpp"

namespace value { namespace debug {
    
    const char* nonstandard_exception::what() const noexcept {
        return "no message";
    }


    std::string tidy_exception_name(const std::exception& e)
    {
        if (typeid(e) == typeid(nonstandard_exception))
        {
            return "nonstandard_exception";
        }
        else
        {
            return strip_nested(demangle(typeid(e)));
        }
    }

    void indenting_formatter::operator()(std::ostream& os, const std::exception& e, size_t depth) const {
        if (depth) os << std::endl;
        os << std::string(depth, ' ') << tidy_exception_name(e) << " : " << e.what();
    }

}}