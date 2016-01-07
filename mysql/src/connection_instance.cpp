#include "connection_instance.hpp"
#include "connection_instance_impl.hpp"
#include "connection_instance_pool.hpp"

#include "error.hpp"

#include <cassert>

namespace value { namespace mysql {

    
    connection_instance::connection_instance(const connection_invariants& invariants)
    : _impl(std::make_shared<impl>(invariants))
    {
    }
    
    connection_instance::connection_instance(create_from_impl, impl_ptr ptr)
    : _impl(std::move(ptr))
    {
        assert(_impl.get());
    }



    auto connection_instance::close() noexcept -> void
    {
        _impl->close();
    }
    
    auto connection_instance::pings_ok() noexcept -> bool
    {
        return _impl->pings_ok();
    }
    
    auto connection_instance::mysql() const -> MYSQL*
    {
        return _impl->mysql();
    }

    
}}