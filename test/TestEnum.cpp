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

using ResultsIndexer = EnumIndexer<Results, Results::Good, Results::Bad, Results::Ugly, Results::Unimplemented, Results::New>;

constexpr static auto ResultsTable = EnumTable<ResultsIndexer, const char*, int>::make_table(
        std::tuple(Results::Bad, "Bad", -1),
        std::tuple(Results::Unimplemented, "Unimplemented", 88),
        std::tuple(Results::New, "New", -100),
        std::tuple(Results::Good, "Good", 12),
        std::tuple(Results::Ugly, "Ugly", 12893)
);


TEST_CASE("Enum Table") {
    REQUIRE("Good" == std::string(ResultsTable.get<0>().get<Results::Good>()));
    REQUIRE("Unimplemented" == std::string(ResultsTable.get<0>().get<Results::Unimplemented>()));
}
