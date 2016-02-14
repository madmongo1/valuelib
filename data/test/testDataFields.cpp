#include <gtest/gtest.h>
#include <type_traits>

#include "test_utils.hpp"

#include <valuelib/data/enumeration.hpp>
#include <valuelib/data/field.hpp>
#include <valuelib/data/storage.hpp>
#include <valuelib/data/entity.hpp>

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

template<class Tag, class Definition>
struct column
{
    using tag_type = Tag;
    using data_type = Definition;
    
    static constexpr auto identifier() { return tag_type::identifier(); }
};


template<class Tag>
struct table {
    using tag_type = Tag;
};

template<class...Columns>
struct column_list
{
    
};


namespace structure {
struct tbl_session
{
    struct login_state : value::data::column<login_state, value::data::default_storage<concepts::login_state> >
    {
        IDENTIFIED_BY(login_state);
    };
    
    struct password_hash : value::data::column<password_hash, value::data::default_storage<concepts::password_hash> >
    {
        IDENTIFIED_BY(password_hash);
    };
    
    struct username : value::data::column<username, value::data::default_storage<concepts::username> >
    { IDENTIFIED_BY(username); };
    
    struct last_seen : value::data::column<last_seen, value::data::default_storage<concepts::last_seen> >
    { IDENTIFIED_BY(last_seen); };
    
    struct session_cookie : value::data::column<session_cookie, value::data::default_storage<concepts::session_cookie> >
    { IDENTIFIED_BY(session_cookie)};
    
    using columns = value::data::column_list<login_state, password_hash>;
    
    using primary_key = value::data::index<value::data::column_ref<session_cookie>>;
    
    using indexes = value::data::index_list<>;
};
}

struct tbl_session
{
    struct login_state_id { IDENTIFIED_BY(login_state); };
    using login_state = value::data::column<login_state_id, value::data::default_storage<concepts::login_state> >;

    struct password_hash : value::data::column<password_hash, value::data::default_storage<concepts::password_hash> >
    {
        IDENTIFIED_BY(password_hash);
    };

    struct username : value::data::column<username, value::data::default_storage<concepts::username> >
    { IDENTIFIED_BY(username); };

    struct last_seen : value::data::column<last_seen, value::data::default_storage<concepts::last_seen> >
    { IDENTIFIED_BY(last_seen); };

    struct session_cookie : value::data::column<session_cookie, value::data::default_storage<concepts::session_cookie> >
    { IDENTIFIED_BY(session_cookie)};

    using columns = value::data::column_list<login_state, password_hash>;

    using primary_key = value::data::index<value::data::column_ref<session_cookie>>;

    using indexes = value::data::index_list<>;


    using structure = structure::tbl_session;
    IDENTIFIED_BY(tbl_session);
    using definition = value::data::table<tbl_session, structure::columns, structure::primary_key, structure::indexes>;

};


/*
 struct session_table
 : value::data::table_entity
 <
 value::data::field_list<session_cookie, login_state, username, password_hash, last_seen>,
 value::data::index< value::data::ascending<session_cookie> >,
 value::data::index_list<>
 >
 {
 
 };
 */

struct nothing {
    virtual void foo() = 0;
    virtual ~nothing() = default;
};

struct something : nothing {
    void foo() override
    {
        static constexpr auto login_state_def = tbl_session::login_state();
        emit(login_state_def.identifier());
    }
};
std::unique_ptr<nothing> make_something(std::string s)
{
    if (s == "something")
        return std::unique_ptr<nothing> { new something };
    else
        throw std::logic_error("error");
}

TEST(testStorage, testBasics)
{
    std::istringstream ss("something");
    make_something(ss.str())->foo();
}


    struct tbl_session
    {
        static constexpr auto identifier() { return value::immutable::string("tbl_session"); }
    };




static constexpr auto session_table = make_table_entity("tbl_session"),
make_columns(make_column("login_state", value::data::default_storage<concepts::login_state>)),
make_primary_key(),
make_indexes()));
{
    DEFINE_IDENTIFIER("tbl_session");
    DEFINE_COLUMN(login_state, value::data::default_storage<concepts::login_state>);
    DEFINE_COLUMN(password_hash, value::data::default_storage<concepts::password_hash>);
    DEFINE_COLUMN(username, value::data::default_storage<concepts::username>);
    DEFINE_COLUMN(last_seen, value::data::default_storage<concepts::last_seen>);
    DEFINE_COLUMN(session_cookie, value::data::default_storage<concepts::session_cookie>);

    using biography_storage = value::data::string_storage<concepts::biography, value::data::unlimited_length, value::data::nullable>;
    DEFINE_COLUMN(biography, biography_storage);

    using columns = std::tuple<login_state, password_hash, username, last_seen, session_cookie, biography>;
    using primary_key = std::tuple<session_cookie>;
};

TEST(sql_generation, mysql)
{

}