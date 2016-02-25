//
//  posix_time.hpp
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//
#pragma once

#include <boost/date_time/posix_time.hpp>

namespace value { namespace data {
  
    template<>
    struct storage_traits<boost::posix_time::ptime>
    {
        using connector_write_interface = std::tuple<std::string>;
        using connector_read_interface = std::tuple<std::string>;
        static constexpr auto default_limits() { return no_size_limits(); }
    };
    

}}