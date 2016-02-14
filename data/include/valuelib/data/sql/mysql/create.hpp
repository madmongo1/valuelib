#pragma once

#include <valuelib/data/entity.hpp>
#include <valuelib/immutable/string.hpp>

namespace value { namespace data { namespace sql { namespace mysql {

    template<class Entity>
    static constexpr sql_create(Entity);

    template<class Identifier, class ColumnList, class PrimaryKeyIndex, class Indexes>
    static constexpr auto sql_create<table<Identifier, ColumnList, PrimaryKeyIndex, Indexes> table)
    {
        static constexpr sql = immutable::string("CREATE TABLE IS NOT EXISTS ") +
        backtick(table.identifier());

    }



}}}}