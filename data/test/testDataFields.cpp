#include <gtest/gtest.h>
#include <type_traits>

#include "test_utils.hpp"

#include <valuelib/data/enumeration.hpp>
#include <valuelib/data/field.hpp>
#include <valuelib/data/storage.hpp>
#include <valuelib/data/entity.hpp>

#include <valuelib/data/sql/mysql/dialect.hpp>
#include <valuelib/data/identifier.hpp>

#include <valuelib/debug/demangle.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <valuelib/tuple/print.hpp>

VALUE_DATA_DEFINE_FIELD(foo, std::string);
VALUE_DATA_DEFINE_FIELD(bar, std::string);

VALUE_DATA_DEFINE_ENUM(Name, (x)(y)(z));

TEST(testDataItem, testMap)
{
    constexpr auto assignable_from_char_star = std::is_assignable<foo, const char*>::value;
    EXPECT_FALSE(assignable_from_char_star);
    
    constexpr auto assignable_from_string = std::is_assignable<foo, std::string>::value;
    EXPECT_FALSE(assignable_from_string);
    
    constexpr auto assignable_from_bar = std::is_assignable<foo, bar>::value;
    EXPECT_FALSE(assignable_from_bar);
    
    constexpr auto assignable_from_foo = std::is_assignable<foo, foo>::value;
    EXPECT_TRUE(assignable_from_foo);
    
    auto f = foo("hello");
    auto b = bar{"world"};
    
    auto f2 = foo("igloo");
    
    EXPECT_GT(f2, f);
    
    
    // efficient to_string
    
    EXPECT_TRUE(std::addressof(f.value()) == std::addressof(to_string(f)));
    
    Name x(std::string("y"));
    
    EXPECT_EQ(x, Name::value_type::y);
    EXPECT_THROW(x = std::string("foo"), std::invalid_argument);
    auto s = to_string(x);
    EXPECT_EQ(std::string("y"), s);
    
    constexpr auto is_enum = std::is_enum<Name::value_type>::value;
    EXPECT_TRUE(is_enum);
    
}

namespace concepts {
    VALUE_DATA_DEFINE_ENUM(login_state, (logged_out)(logged_in));
    VALUE_DATA_DEFINE_FIELD(user_id, std::string);
    VALUE_DATA_DEFINE_FIELD(password_hash, std::string);
    VALUE_DATA_DEFINE_FIELD(session_mru, boost::posix_time::ptime);
    VALUE_DATA_DEFINE_FIELD(session_cookie, boost::uuids::uuid);
    VALUE_DATA_DEFINE_FIELD(biography, std::string);
}

#define DEFINE_IDENTIFIER(Identifier) \
static constexpr auto identifier() { return value::immutable::string(#Identifier); }

#define DEFINE_COLUMN(ColumnName, Storage) \
struct ColumnName { \
DEFINE_IDENTIFIER(ColumnName); \
using storage_type = Storage; \
}


#define IDENTIFIED_BY(Identifier) static const auto constexpr identifier() { return value::immutable::string(#Identifier); }


struct tbl_session : value::data::table<tbl_session>
{
    VALUE_DATA_IDENTIFIED_BY(tbl_session);
    
    struct login_state : value::data::column<login_state>
    {
        VALUE_DATA_IDENTIFIED_BY(login_state);
        static constexpr auto storage() {
            return value::data::default_storage<concepts::login_state>();
        }
        static constexpr auto default_value() {
            return value::data::default_from_native();
        }
    };
    
    struct session_mru : value::data::column<session_mru> {
        VALUE_DATA_IDENTIFIED_BY(session_mru);
        static constexpr auto storage() {
            return value::data::default_storage<concepts::session_mru>();
        }
        static constexpr auto default_value() {
            return value::data::current_timestamp();
        }
        
    };
    
    struct session_cookie : value::data::column<session_cookie>
    {
        VALUE_DATA_IDENTIFIED_BY(session_cookie);
        static constexpr auto storage() {
            return value::data::default_storage<concepts::session_cookie>();
        }
    };
    
    struct user_id : value::data::column<user_id>
    {
        VALUE_DATA_IDENTIFIED_BY(user_id);
        static constexpr auto storage() {
            return value::data::string_storage<
            concepts::user_id,
            value::data::limited_length<250>,
            value::data::nullable>();
        }
    };
    
    static constexpr auto columns() {
        return std::make_tuple(session_cookie(),
                               login_state(),
                               session_mru(),
                               user_id());
    }
    
    static constexpr auto primary_key() {
        return std::make_tuple(value::data::column_ref<session_cookie>());
    }
    
};

TEST(testStorage, testBasics)
{
    using namespace std::string_literals;
    
    static constexpr auto sql_create = value::data::sql::mysql::sql_create(tbl_session());
    EXPECT_EQ("CREATE TABLE IF NOT EXISTS `tbl_session`(\n"
              "`session_cookie` VARCHAR(36) NOT NULL,\n"
              "`login_state` SET('logged_out','logged_in') NOT NULL DEFAULT 'logged_out',\n"
              "`session_mru` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\n"
              "`user_id` VARCHAR(250) NULL"
              ",\nPRIMARY KEY (`session_cookie`)\n"
              ") ENGINE=InnoDB DEFAULT CHARSET=utf8",
              sql_create);
    
}

TEST(testStorage, testNativeDeduction)
{
    using session_cookie_native = value::data::DeduceNativeArgument<tbl_session::session_cookie>;
    
    EXPECT_STREQ(typeid(concepts::session_cookie).name(),
                 typeid(session_cookie_native).name()) << value::debug::demangle(typeid(session_cookie_native));
    
    using user_id_native = value::data::DeduceNativeArgument<tbl_session::user_id>;
    
    EXPECT_STREQ(typeid(boost::optional<concepts::user_id>).name(),
                 typeid(user_id_native).name()) << value::debug::demangle(typeid(user_id_native));
    
    using session_mru_native = value::data::DeduceNativeArgument<tbl_session::session_mru>;
    
    EXPECT_STREQ(typeid(concepts::session_mru).name(),
                 typeid(session_mru_native).name()) << value::debug::demangle(typeid(session_mru_native));
    
    using cols = decltype(tbl_session::columns());
    using native_cols = value::data::DeduceNativeArguments<cols>;
    EXPECT_STREQ(typeid(std::tuple<
                        concepts::session_cookie,
                        concepts::login_state,
                        concepts::session_mru,
                        boost::optional<concepts::user_id>
                        >).name(),
                 typeid(native_cols).name()) << value::debug::demangle(typeid(native_cols).name());
    
    
    
}


TEST(testStorage, testUnderlyingType)
{
    using u1 = value::data::UnderlyingType<value::data::DeduceNativeArgument<tbl_session::session_cookie>>;
    EXPECT_STREQ(typeid(boost::uuids::uuid).name(),
                 typeid(u1).name()) << value::debug::demangle(typeid(u1));
    
    using u2 = value::data::UnderlyingType<value::data::DeduceNativeArgument<tbl_session::session_mru>>;
    EXPECT_STREQ(typeid(boost::posix_time::ptime).name(),
                 typeid(u2).name()) << value::debug::demangle(typeid(u2));
    
    using u3 = value::data::UnderlyingType<value::data::DeduceNativeArgument<tbl_session::login_state>>;
    EXPECT_STREQ(typeid(concepts::login_state::value_type).name(),
                 typeid(u3).name()) << value::debug::demangle(typeid(u3));
    
    using u4 = value::data::UnderlyingType<value::data::DeduceNativeArgument<tbl_session::user_id>>;
    EXPECT_STREQ(typeid(std::string).name(),
                 typeid(u4).name()) << value::debug::demangle(typeid(u4));
    
}

TEST(test_io, streaming_binary)
{
    using namespace std;
    using vector_type = std::vector<std::uint8_t>;
    auto v = vector_type(256);
    std::generate_n(std::begin(v), 256, [val = uint8_t(0)]() mutable { return val++; });
    
    std::ostringstream oss;
    oss.write(reinterpret_cast<const char*>(v.data()), v.size());
    
    auto written = oss.str();
    auto write_check = vector_type(begin(written), end(written));
    
    ASSERT_EQ(v, write_check);
    
    std::istringstream iss(written);
    auto v2 = vector_type();
    iss >> noskipws;
    copy(istream_iterator<uint8_t>(iss), istream_iterator<uint8_t>(), back_inserter(v2));
    
    ASSERT_EQ(v, v2);
}

#include <valuelib/data/storable_data.hpp>

TEST(test_io, native_binary_type)
{
    struct type1 : value::data::storable_data<type1, std::vector<std::uint8_t>> {
        using storable_data_type::storable_data_type;
        static constexpr auto identifier() { return value::immutable::string("type1"); }
    };
    
    struct type2 : value::data::storable_data<type2, std::vector<std::uint8_t>> {
        using storable_data_type::storable_data_type;
        static constexpr auto identifier() { return value::immutable::string("type2"); }
    };
    
    ASSERT_TRUE(value::data::IsStorableData<type1>);
    
    ASSERT_FALSE(( std::is_constructible<type1, type2>::value ));
    ASSERT_TRUE(( std::is_constructible<type1, type1::underlying_type>::value ));
    ASSERT_TRUE(( std::is_copy_constructible<type1>::value ));
    ASSERT_TRUE(( std::is_copy_assignable<type1>::value ));
    
    auto v = type1(type1::underlying_type(256));
    std::generate_n(std::begin(v.value()), 256, [val = uint8_t(0)]() mutable { return val++; });
    
    std::ostringstream ss;
    ss << value::tuple::print_tuple(v);
    ASSERT_EQ("type1 : [ 000102030405060708090a0b0c0d0e0f 101112131415161718191a1b1c1d1e1f 202122232425262728292a2b2c2d2e2f 303132333435363738393a3b3c3d3e3f\n  404142434445464748494a4b4c4d4e4f 505152535455565758595a5b5c5d5e5f 606162636465666768696a6b6c6d6e6f 707172737475767778797a7b7c7d7e7f\n  808182838485868788898a8b8c8d8e8f 909192939495969798999a9b9c9d9e9f a0a1a2a3a4a5a6a7a8a9aaabacadaeaf b0b1b2b3b4b5b6b7b8b9babbbcbdbebf\n  c0c1c2c3c4c5c6c7c8c9cacbcccdcecf d0d1d2d3d4d5d6d7d8d9dadbdcdddedf e0e1e2e3e4e5e6e7e8e9eaebecedeeef f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff ]", ss.str());

    ASSERT_TRUE(( value::tuple::TupleHasType<std::tuple<std::ostream>, std::ostream>));
    ASSERT_TRUE(( value::tuple::TupleHasType<value::data::StorageTraits<std::vector<std::uint8_t>>::connector_write_interface, std::ostream>));
    
    EXPECT_TRUE(( value::tuple::TupleHasType<value::data::StorageTraits<std::vector<uint8_t>>::connector_write_interface, std::ostream> ));
    ASSERT_TRUE(( value::data::is_derived_from_template_v<type1, value::data::storable_data> ));
    using traits = value::data::StorageTraits<type1::underlying_type>::connector_write_interface;
    std::cerr << value::debug::demangle(typeid(traits)) << std::endl;
    
    ASSERT_TRUE(( value::data::SupportsWrite<type1::underlying_type, std::ostream> ));

    std::stringstream ss2;
    value::data::write_to_connector(ss2, v);
    
    
    auto v2 = value::data::read_from_connector<type1>(ss2);
    ASSERT_EQ(v, v2);
    ASSERT_EQ(v, v2.value());
    ASSERT_LE(v, v2);
    ASSERT_GE(v, v2);
    
    v2.value().at(255) = 0;
    ASSERT_LE(v2, v);
    ASSERT_LT(v2, v);
    ASSERT_GT(v, v2);
    ASSERT_GE(v, v2);
    ASSERT_NE(v, v2);
}

TEST(test_io, string_types)
{
    struct type1 : value::data::storable_data<type1, std::string> {
        using storable_data_type::storable_data_type;
        static constexpr auto identifier() { return value::immutable::string("type1"); }
    };
    
    struct type2 : value::data::storable_data<type2, std::string> {
        using storable_data_type::storable_data_type;
        static constexpr auto identifier() { return value::immutable::string("type2"); }
    };
    
    ASSERT_TRUE(value::data::IsStorableData<type1>);
    
    ASSERT_FALSE(( std::is_constructible<type1, type2>::value ));
    ASSERT_TRUE(( std::is_constructible<type1, type1::underlying_type>::value ));
    ASSERT_TRUE(( std::is_copy_constructible<type1>::value ));
    ASSERT_TRUE(( std::is_copy_assignable<type1>::value ));
    
    auto v = type1(std::string("hello"));
    
    std::ostringstream ss;
    ss << value::tuple::print_tuple(v);
    ASSERT_EQ("type1 : \"hello\"", ss.str());
    
    ASSERT_TRUE(( value::data::is_derived_from_template_v<type1, value::data::storable_data> ));
    ASSERT_TRUE(( value::data::SupportsWrite<type1::underlying_type, std::ostream> ));
    
    std::stringstream ss2;
    value::data::write_to_connector(ss2, v);
    
    
    auto v2 = value::data::read_from_connector<type1>(ss2);
    ASSERT_EQ(v, v2);
    ASSERT_EQ(v, v2.value());
    ASSERT_LE(v, v2);
    ASSERT_GE(v, v2);
    
    v2.value().at(0) = 'a';
    ASSERT_LE(v2, v);
    ASSERT_LT(v2, v);
    ASSERT_GT(v, v2);
    ASSERT_GE(v, v2);
    ASSERT_NE(v, v2);
}

