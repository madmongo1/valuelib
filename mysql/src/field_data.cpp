#include <value/mysql/types.hpp>


namespace value { namespace mysql {
    
    namespace {
        template<class T>
        struct std_to_string_is_value
        {
            
            template<class U> static auto test(U*p) -> decltype(std::to_string(*p),
                                                         void(),
                                                         std::true_type());
            
            template<class U> static auto test(...) -> decltype(std::false_type());
            
            static constexpr bool value = decltype(test<T>(nullptr))::value;
        };
        
        template<class T>
        static constexpr bool StdToStringIsValid = std_to_string_is_value<T>::value;
    }
    
    struct convert_to_string : boost::static_visitor<std::string>
    {
        auto operator()(const std::string& str) const {
            return str;
        }
        
        auto operator()(std::string& str) const {
            return std::string(std::move(str));
        }

        template<
        class T,
        std::enable_if_t< StdToStringIsValid<T> > * = nullptr
        >
        auto operator()(T v) const {
            return std::to_string(v);
        }
        
        auto operator()(const blob_data& data) const {
            std::string result;
            result.reserve(data.size());
            std::transform(begin(data),
                           end(data),
                           std::back_inserter(result),
                           [](auto b) {
                               return static_cast<char>(b);
                           });
            return result;
        }
        
    };
    
    auto to_string(const field_data& fd) -> std::string
    {
        return boost::apply_visitor(convert_to_string(), fd);
    }
    
    auto to_string(field_data&& fd) -> std::string
    {
        return boost::apply_visitor(convert_to_string(), fd);
    }
    
    
    
    
}}