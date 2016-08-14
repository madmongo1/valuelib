#include <valuelib/debug/trace.hpp>

namespace value { namespace debug {
  
    __thread std::size_t trace_level::_current_depth = 0;
    
}}