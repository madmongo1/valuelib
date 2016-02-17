#include <gtest/gtest.h>
#include <type_traits>

#include "test_utils.hpp"

#include <valuelib/data/enumeration.hpp>
#include <valuelib/data/field.hpp>
#include <valuelib/data/storage.hpp>
#include <valuelib/data/entity.hpp>

#include <valuelib/data/sql/mysql/create.hpp>

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
    VALUE_DATA_DEFINE_FIELD(username, std::string);
    VALUE_DATA_DEFINE_FIELD(password_hash, std::string);
    VALUE_DATA_DEFINE_FIELD(last_seen, boost::posix_time::ptime);
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
    IDENTIFIED_BY(tbl_session);

    struct login_state_identifier { IDENTIFIED_BY(login_state); };
    using login_state = value::data::column<
    login_state_identifier,
    value::data::default_storage<concepts::login_state>,
    value::data::default_from_native
    >;

    struct last_seen_identifier { IDENTIFIED_BY(last_seen); };
    using last_seen = value::data::column<
    last_seen_identifier,
    value::data::default_storage<concepts::last_seen>,
    value::data::current_timestamp
    >;
    
    struct session_cookie_tag { IDENTIFIED_BY(session_cookie); };
    using session_cookie = value::data::column<session_cookie_tag, value::data::default_storage<concepts::session_cookie> >;
    
    static constexpr auto columns() { return std::make_tuple(session_cookie(),
                                                             login_state(),
                                                             last_seen()); }

    static constexpr auto primary_key() {
        return std::make_tuple(value::data::column_ref<session_cookie>());
    }
    
    using indexes = value::data::index_list<>;
    
};


TEST(testStorage, testBasics)
{
    using namespace std::string_literals;
    
    static constexpr auto sql_create = value::data::sql::mysql::sql_create(tbl_session());
    EXPECT_EQ("CREATE TABLE IF NOT EXISTS `tbl_session`(\n"
              "`session_cookie` VARCHAR(36) NOT NULL,\n"
              "`login_state` SET('logged_out','logged_in') NOT NULL DEFAULT 'logged_out',\n"
              "`last_seen` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\n"
              "PRIMARY KEY (`session_cookie`)\n"
              ") ENGINE=InnoDB DEFAULT CHARSET=utf8",
              sql_create);
    
}
