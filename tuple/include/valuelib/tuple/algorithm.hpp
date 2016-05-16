//
//  metafunction.hpp
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//

#pragma once
#include <tuple>
#include <utility>

namespace value { namespace tuple {
    
    namespace impl
    {
        //! Tests if T is a specialization of Template
        template <typename T, template <typename...> class Template>
        struct is_specialization_of : std::false_type {};
        
        template <template <typename...> class Template, typename... Args>
        struct is_specialization_of<Template<Args...>, Template> : std::true_type {};
    }
    template<typename T, template <typename...> class Template>
    static constexpr auto IsSpecializationOf = impl::is_specialization_of<std::decay_t<T>, Template>::value;

    
    template<class Func, class...Args>
    void map(Func&& func, Args&&...args)
    {
#if __cplusplus > 201402L
        (std::forward<Func>(func)(std::forward<Args>(args)),...);
#else
        using expand = int[];
        void(expand{0,
            (std::forward<Func>(func)(std::forward<Args>(args))),
             0)...
        });
#endif
        
    }

    namespace impl
    {
        template<class Func, class Tuple, std::size_t...Is>
        void for_each(Tuple&& tuple, Func&& func, std::index_sequence<Is...>)
        {
#if __cplusplus > 201402L
            (std::forward<Func>(func)(std::get<Is>(std::forward<Tuple>(tuple))),...);
#else
            using expand = int[];
            void(expand{0,
                (std::forward<Func>(func)(std::get<Is>(std::forward<Tuple>(tuple))),
                 0)...
            });
#endif

        }
    }
    
    template<
    class Func,
    class Tuple,
    std::enable_if_t<IsSpecializationOf<Tuple, std::tuple>>* = nullptr
    >
    void for_each(Tuple&& tuple, Func&& func)
    {
        using tuple_type = std::decay_t<Tuple>;
        static constexpr auto tsize = std::tuple_size<tuple_type>::value;
        
        return impl::for_each(std::forward<Tuple>(tuple),
                              std::forward<Func>(func),
                              std::make_index_sequence<tsize>());
    }
    
    
    namespace impl
    {
        template<class Value, class Arg, class Op>
        void apply_operation(Op& op, Value& acc, Arg&& arg)
        {
            acc = op(acc, std::forward<Arg>(arg));
        }
        
        template<class Value, class Tuple, class Operation, std::size_t...Is>
        void accumulate(Tuple&& tuple, Value& acc, Operation op, std::index_sequence<Is...>)
        {
#if __cplusplus > 201402L
            (apply_operation(op, acc, std::get<Is>(std::forward<Tuple>(tuple))),...);
#else
            using expand = int[];
            void(expand{0, (
                            apply_operation(op, acc, std::get<Is>(std::forward<Tuple>(tuple))),
                            0)...});
       
#endif
        }
    }
    
    template<
    class Value,
    class Tuple,
    class Operation = std::plus<>,
    std::enable_if_t<IsSpecializationOf<Tuple, std::tuple>>* = nullptr
    >
    constexpr Value accumulate(Tuple&& tuple, Value val, Operation&& op = Operation())
    {
        using tuple_type = std::decay_t<Tuple>;
        constexpr auto tsize = std::tuple_size<tuple_type>::value;
        
        impl::accumulate(std::forward<Tuple>(tuple),
                         val,
                         std::forward<Operation>(op),
                         std::make_index_sequence<tsize>());
        return val;
    }
    
    namespace impl
    {
        template<class Collector, class Tuple, std::size_t...Is>
        void collect(Tuple&& tuple, Collector& col, std::index_sequence<Is...>)
        {
#if __cplusplus > 201402L
            (col(std::get<Is>(std::forward<Tuple>(tuple))),...);
#else
            using expand = int[];
            void(expand{0, (
                            col(std::get<Is>(std::forward<Tuple>(tuple))),
                            0)...});
            
#endif
        }
    }
    
    template<
    class Tuple,
    class Collector,
    std::enable_if_t<IsSpecializationOf<Tuple, std::tuple>>* = nullptr
    >
    constexpr Collector collect(Tuple&& tuple, Collector col)
    {
        using tuple_type = std::decay_t<Tuple>;
        constexpr auto tsize = std::tuple_size<tuple_type>::value;
        
        impl::collect(std::forward<Tuple>(tuple),
                         col,
                         std::make_index_sequence<tsize>());
        return col;
    }

}}