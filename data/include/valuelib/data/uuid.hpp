//
//  uuid.hpp
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//
#pragma once
#include <valuelib/data/storable_data.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace value { namespace data {
    
    template<>
    struct storage_traits<boost::uuids::uuid>
    {
        using connector_write_interface = std::tuple<std::string>;
        using connector_read_interface = std::tuple<std::string>;
        static constexpr auto default_limits() { return no_size_limits(); }
    };

    namespace impl {
        
        template<class Identifier>
        struct to_string<storable_data<Identifier, boost::uuids::uuid>>
        {
            using storable_type = storable_data<Identifier, boost::uuids::uuid>;
            using arg_type = storable_type const&;
            using result_type = std::string;
            
            constexpr to_string() {};
            result_type operator()(arg_type arg) const {
                using boost::uuids::to_string;
                return to_string(arg.value());
            }
        };
        
    }

}}


