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
