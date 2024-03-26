#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "CppUtils/c_util/Enum.h"

#include <iostream>
#include <string_view>


#include "CppUtils/preproc/VariadicMacros.h"

INDEXED_ENUM(Results,
    Good,
    Bad,
    Ugly,
    Unimplemented,
    New
);

INDEXED_ENUM(ResultsFields,
    Name,
    Value
);

constexpr static auto ResultsTable = EnumTable<ResultsIndexer, ResultsFieldsIndexer, std::string_view, int>::make_table(
        std::tuple(Results::Bad, "Bad", -1),
        std::tuple(Results::Unimplemented, "Unimplemented", 88),
        std::tuple(Results::New, "New", -100),
        std::tuple(Results::Good, "Good", 12),
        std::tuple(Results::Ugly, "Ugly", 12893)
);

template <Results r>
struct check_result {
    bool operator()(const std::string& s) const { return s == ResultsTable.get<r, ResultsFields::Name>(); }
};

template <Results r>
class handle_result {
public:
    handle_result() = default;

    void operator()(const std::string& s) {
        REQUIRE(s == ResultsTable.get<r, ResultsFields::Name>());
    }
};


TEST_CASE("Enum Table") {
    REQUIRE(ResultsTable.num_entries() == 5);
    REQUIRE(ResultsTable.num_fields() == 2);

    static_assert("Good" == ResultsTable.get<Results::Good, ResultsFields::Name>());
    static_assert("Unimplemented" == ResultsTable.get<Results::Unimplemented, ResultsFields::Name>());
    static_assert(-100 == ResultsTable.get<ResultsFields::Value>().get<Results::New>());
    static_assert(Results::Ugly == ResultsTable.lookup<ResultsFields::Name>("Ugly"));

    int value = *ResultsTable.get<ResultsFields::Value>(Results::Bad);
    REQUIRE(value == -1);

    //ResultsIndexer::dispatch<handle_result>(Results::Good)("Good");
    auto check = ResultsIndexer::dispatch<check_result>(Results::Good, "Good");
    REQUIRE(check);
    REQUIRE(*check);

    ResultsIndexer::dispatch<handle_result>(Results::New, "New");
}
