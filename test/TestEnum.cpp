#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "CppUtils/c_util/Enum.h"

#include <iostream>
#include <string_view>

enum class Results {
    Good,
    Bad,
    Ugly,
    Unimplemented,
    New,
};

enum class ResultsFields {
    Name,
    Value,
};

using ResultsIndexer = EnumIndexer<Results, Results::Good, Results::Bad, Results::Ugly, Results::Unimplemented, Results::New>;
using ResultsFieldsIndexer = EnumIndexer<ResultsFields, ResultsFields::Name, ResultsFields::Value>;

constexpr static auto ResultsTable = EnumTable<ResultsIndexer, ResultsFieldsIndexer, std::string_view, int>::make_table(
        std::tuple(Results::Bad, "Bad", -1),
        std::tuple(Results::Unimplemented, "Unimplemented", 88),
        std::tuple(Results::New, "New", -100),
        std::tuple(Results::Good, "Good", 12),
        std::tuple(Results::Ugly, "Ugly", 12893)
);


TEST_CASE("Enum Table") {
    REQUIRE(ResultsTable.num_entries() == 5);
    REQUIRE(ResultsTable.num_fields() == 2);

    static_assert("Good" == ResultsTable.get<Results::Good, ResultsFields::Name>());
    static_assert("Unimplemented" == ResultsTable.get<Results::Unimplemented, ResultsFields::Name>());
    static_assert(-100 == ResultsTable.get<ResultsFields::Value>().get<Results::New>());
    static_assert(Results::Ugly == ResultsTable.lookup<ResultsFields::Name>("Ugly"));

    int value = *ResultsTable.get<ResultsFields::Value>(Results::Bad);
    REQUIRE(value == -1);
}
