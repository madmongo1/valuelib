#pragma once

namespace value { namespace data {

    template<class Thing, typename Enable = void>
    struct deduce_native_argument;
    
    template<class Thing> using DeduceNativeArgument = typename deduce_native_argument<Thing>::result;
    
    
    
    template<class Sequence>
    struct deduce_native_arguments;
    
    template<class...Fields>
    struct deduce_native_arguments<std::tuple<Fields...>>
    {
        using result = std::tuple< DeduceNativeArgument<Fields> ... >;
    };
    
    template<class Sequence> using DeduceNativeArguments = typename deduce_native_arguments<Sequence>::result;

}}