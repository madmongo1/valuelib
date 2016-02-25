#pragma once
#include <boost/optional/optional_fwd.hpp>
#include <utility>
#include <valuelib/debug/demangle.hpp>
#include <iterator>

namespace value { namespace tuple {

    namespace detail {
        template<typename S, typename T>
        class is_streamable
        {
            template<typename SS, typename TT>
            static auto test(int)
            -> decltype( std::declval<SS&>() << std::declval<TT>(), std::true_type() );
            
            template<typename, typename>
            static auto test(...) -> std::false_type;
            
        public:
            static const bool value = decltype(test<S,T>(0))::value;
        };
    }
    
    template<class L, class R> constexpr auto SupportsOutStream = detail::is_streamable<L, R>::value;

    /// Declare the concept of a tuple_printer
    template<class T, typename Enable = void> struct tuple_printer;
    
    namespace detail
    {
        template<class T>
        struct tuple_printer_exists {
            template<class U>
            static auto test(int)
            -> decltype( tuple_printer<U>(std::declval<U>()), std::true_type() );
            
            template<typename> static auto test(...) -> std::false_type;
            static constexpr auto value = decltype(test<T>(0))::value;
        };
    }
    
    template <class T>
    static constexpr auto TuplePrinterExists = detail::tuple_printer_exists<std::decay_t<T>>::value;

    /// Print any object as a tuple.
    /// By default, objects that support operator<< are simply returned unchanged
    /// If a model of value::tuple::impl::tuple_printer<T> is available, this will be 
    /// returned
    template<class T, std::enable_if_t<TuplePrinterExists<T>>* = nullptr>
    auto print_tuple(const T& obj)
    {
        return tuple_printer<std::decay_t<T>>(obj);
    }

    template<class Iter>
    struct range
    {
        range(Iter first, Iter last)
        : _begin(first)
        , _end(last)
        {}
        
        auto begin() const { return _begin; }
        auto end() const { return _end; }
        
    private:
        Iter _begin, _end;
    };
    
    /// specialisation for ranges
    template<class Iter> auto begin(const range<Iter>& r) { return r.begin(); }
    template<class Iter> auto end(const range<Iter>& r) { return r.end(); }
    template<class Iter> auto make_range(Iter first, Iter last)
    {
        return range<std::decay_t<Iter>>(first, last);
    }
    
    template<class Container> auto make_range(const Container& c) {
        return range<typename Container::const_iterator>(std::begin(c), std::end(c));
    }
    template<class Container> auto make_range(Container& c) {
        return range<typename Container::iterator>(std::begin(c), std::end(c));
    }
    template<class Container> using Range = decltype(make_range(std::declval<Container>()));

    template<class Iter>
    decltype(auto) print_tuple(Iter first, Iter last)
    {
        return tuple_printer<range<Iter>>(make_range(first, last));
    }

    template<class T, std::enable_if_t<not TuplePrinterExists<T>>* = nullptr>
    decltype(auto) print_tuple(const T& obj)
    {
        using namespace std;
        return "unprintable: "s + value::debug::demangle(typeid(obj));
    }
    
    /// The universal tuple_printer.
    template<class T, typename Enable>
    struct tuple_printer
    {
        using argument_type = T;
        
        constexpr tuple_printer(const argument_type& arg) noexcept
        : _arg(arg) {}
        
        template<
        class Stream
        >
        decltype(auto) operator()(Stream& os) const {
            return os << _arg;
        }
        
        const argument_type& _arg;
    };
    
    template<class Stream, class T, typename Enable>
    decltype(auto) operator<<(Stream& os, const tuple_printer<T, Enable>& printer)
    {
        return printer(os);
    }
    


    /// Specialisation of tuple_printer for printing a tuple
    
    template<class...Ts>
    struct tuple_printer<std::tuple<Ts...>>
    {
        using argument_type = std::tuple<Ts...>;
        
        constexpr tuple_printer(argument_type const& arg)
        : _arg(std::addressof(arg))
        {}
        
        template<class Stream>
        static void comma(Stream& os, size_t i)
        {
            if (i)
                os << ", ";
        }
        
        template<class Stream, size_t...Is>
        void emit(Stream& os, std::index_sequence<Is...>) const
        {
            (
             (
              comma(os, Is),
              (os << print_tuple(std::get<Is>(*_arg)))
              ),
             ...);
        }
        
        template<class Stream>
        Stream& operator()(Stream& os) const
        {
            os << "(";
            emit(os, std::make_index_sequence<sizeof...(Ts)>());
            os << ")";
            return os;
        }
        
        argument_type const* _arg;
    };
    
    /// specialisation for printing a pair
    template<class T1, class T2>
    struct tuple_printer< std::pair<T1, T2> >
    {
        using arg_type = std::pair<T1, T2>;
        tuple_printer(const arg_type& arg) : _arg(arg) {}
        
        template<class Stream>
        decltype(auto) operator()(Stream& os) const {
            return os << "(" << print_tuple(_arg.first) << ", " << print_tuple(_arg.second) << ")";
        }
        private:
        const arg_type& _arg;
    };
    
    /// specialisation for printing a string
    template<class Ch, class Traits, class Alloc>
    struct tuple_printer<std::basic_string<Ch, Traits, Alloc>>
    {
        using arg_type = std::basic_string<Ch, Traits, Alloc>;
        tuple_printer(arg_type const& arg) : _arg(arg) {}
        
        template<class Stream>
        decltype(auto) operator()(Stream& s) const
        {
            return s << std::quoted(_arg);
        }
    private:
        arg_type const& _arg;
    };
    
    template<>
    struct tuple_printer<const char*>
    {
        using arg_type = const char*;
        tuple_printer(arg_type const& arg) : _arg(arg) {}
        
        template<class Stream>
        decltype(auto) operator()(Stream& s) const
        {
            return s << std::quoted(_arg);
        }
    private:
        arg_type _arg;
    };
    
    template<class Iter>
    using ValueOfIterator = typename std::iterator_traits<std::decay_t<Iter>>::value_type;
    
    // version for ranges that are not binary data
    template<class Iter>
    struct tuple_printer
    <
    range<Iter>
    ,
    std::enable_if_t<
    not std::is_same<std::uint8_t, ValueOfIterator<Iter>>::value
    >
    >
    {
        using arg_type = range<Iter>;
        tuple_printer(const arg_type& arg) : _arg(arg) {}
        
        template<class Stream>
        decltype(auto) operator()(Stream& os) const {
            os << "[";
            auto sep = " ";
            for(const auto& e : _arg)
            {
                os << sep << print_tuple(e);
                sep = ", ";
            }
            return os << " ]";
        }
        const arg_type& _arg;
    };
    
    template<class Iter>
    struct tuple_printer
    <
    range<Iter>
    ,
    std::enable_if_t<
    std::is_same<std::uint8_t, ValueOfIterator<Iter>>::value
    >
    >
    {
        using arg_type = range<Iter>;
        tuple_printer(const arg_type& arg) : _arg(arg) {}
        
        template<class Stream>
        decltype(auto) operator()(Stream& os) const {
            os << "[";
            os << std::hex;
            std::size_t count = 0;
            for(const auto& e : _arg)
            {
                if (count && (count % 64 == 0)) os << "\n  ";
                else if (count % 16 == 0) os << " ";
                os << std::setfill('0') << std::setw(2) << unsigned(e);
                ++count;
            }
            return os << " ]";
        }
        const arg_type& _arg;
    };
    
    template<class V, class A>
    struct tuple_printer<std::vector<V,A>>
    {
        using arg_type = std::vector<V,A>;
        tuple_printer(const arg_type& arg) : _range(make_range(arg)) {}
        
        template<class Stream>
        decltype(auto) operator()(Stream& os) const {
            os << print_tuple(_range);
        }
        Range<arg_type> _range;
    };
    
    
    template <typename T, template <typename, typename...> class Tmpl>  // #1 see note
    struct is_derived_from_template
    {
        typedef char yes[1];
        typedef char no[2];
        
        static no & test(...);
        
        template <typename ...U>
        static yes & test(Tmpl<U...> const &);
        
        static bool constexpr value = sizeof(test(std::declval<T>())) == sizeof(yes);
    };
    template<typename T, template <typename, typename...> class Tmpl>
    static constexpr bool is_derived_from_template_v = is_derived_from_template<T, Tmpl>::value;
    
    
    namespace impl {
        // default case - underlying type is the same as type
        template<class T, typename = void>
        struct underlying_type
        {
            using result = T;
        };
    }
    
    template<class T> using UnderlyingType = typename impl::underlying_type<T>::result;
    
    namespace impl {
        template<class T> struct underlying_type<boost::optional<T>, void> {
            using result = UnderlyingType<T>;
        };
    }
    
}}