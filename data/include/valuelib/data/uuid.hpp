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
        
        template<>
        struct to_string_impl<boost::uuids::uuid>
        {
            using arg_type = boost::uuids::uuid const&;
            using result_type = std::string;
            
            result_type operator()(arg_type arg) const {
                using boost::uuids::to_string;
                return to_string(arg);
            }
        };
        
        template<>
        struct from_string_impl<boost::uuids::uuid>
        {
            using result_type = boost::uuids::uuid;
            result_type operator()(const std::string& s) const;
        };
        
    }

}}


