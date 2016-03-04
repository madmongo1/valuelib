//
//  uuid.cpp
//  ProjectX
//
//  Created by Richard Hodges on 04/03/2016.
//
//
#include <valuelib/data/uuid.hpp>
#include <boost/lexical_cast.hpp>


namespace value { namespace data {
  
    namespace impl {

        auto from_string_impl<boost::uuids::uuid>::operator()(const std::string& s) const -> result_type
        {
            return boost::lexical_cast<boost::uuids::uuid>(s);
        }

    }
}}