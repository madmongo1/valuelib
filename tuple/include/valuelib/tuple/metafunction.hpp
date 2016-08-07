//
//  metafunction.hpp
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//

#pragma once

namespace value { namespace tuple {
    
    
    //
    // deduce whether a type exists in a tuple of types
    //
    
    namespace detail {
        template <typename T, typename Tuple>
        struct has_type;
        
        template <typename T>
        struct has_type<T, std::tuple<>> : std::false_type {};
        
        template <typename T, typename U, typename... Ts>
        struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {};
        
        template <typename T, typename... Ts>
        struct has_type<T, std::tuple<T, Ts...>> : std::true_type {};
    }
    template <typename T, typename Tuple>
    using has_type = typename detail::has_type<T, Tuple>::type;
    
    /*
     namespace impl {
     template <typename T, typename Tuple>
     struct tuple_has_type;
     
     template <typename T>
     struct tuple_has_type<T, std::tuple<>> : std::false_type {};
     
     template <typename T, typename U, typename... Ts>
     struct tuple_has_type<T, std::tuple<U, Ts...>> : tuple_has_type<T, std::tuple<Ts...>> {};
     
     template <typename T, typename... Ts>
     struct tuple_has_type<T, std::tuple<T, Ts...>> : std::true_type {};
     }
     */
    template<class Tuple, class Type>
    static constexpr bool TupleHasType = has_type<std::decay_t<Type>, Tuple>::value;
    
}}