#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "CppUtils/c_util/Enum.h"

#include <iostream>


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

constexpr static auto ResultsTable = EnumTable<ResultsIndexer, ResultsFieldsIndexer, const char*, int>::make_table(
        std::tuple(Results::Bad, "Bad", -1),
        std::tuple(Results::Unimplemented, "Unimplemented", 88),
        std::tuple(Results::New, "New", -100),
        std::tuple(Results::Good, "Good", 12),
        std::tuple(Results::Ugly, "Ugly", 12893)
);


TEST_CASE("Enum Table") {
    REQUIRE(ResultsTable.num_entries() == 5);
    REQUIRE(ResultsTable.num_fields() == 2);

    REQUIRE("Good" == std::string(ResultsTable.get<0>().get<Results::Good>()));
    REQUIRE("Unimplemented" == std::string(ResultsTable.get<0>().get<Results::Unimplemented>()));
    REQUIRE(-100 == ResultsTable.get<ResultsFields::Value>().get<Results::New>());
}
