#pragma once

namespace value { namespace stdext {
    
    template<class Container, class Iter, class TransformFunction>
    void assign_transform(Container& target, Iter first, Iter last, TransformFunction func)
    {
        struct transform_iterator : Iter
        {
            using base_class = Iter;
            using value_type = typename Iter::value_type;
            
            transform_iterator(Iter base, TransformFunction& f)
            : base_class(base), func(std::addressof(f))
            {}
            
            value_type operator*() const {
                auto const& src = base_class::operator*();
                return (*func)(src);
            }
            TransformFunction* func;
        };
        
        target.assign(transform_iterator(first, func),
                      transform_iterator(last, func));
    }
    
}}