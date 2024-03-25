#pragma once

#include <type_traits>
#include <array>
#include <cstddef>
#include <tuple>
#include <utility>

template <typename Enum, Enum... EnumValues>
class EnumIndexer {
public:
    using EnumType = Enum;
    constexpr static size_t size = sizeof...(EnumValues);
    constexpr static std::array<Enum, size> values{EnumValues...};

    template <EnumType t>
    constexpr static bool contains() {
        for (EnumType e : values) {
            if (e == t) {
                return true;
            }
        }
        return false;
    }

    template <EnumType t>
    constexpr static size_t get() {
        static_assert(contains<t>());
        for (size_t i = 0; i < size; i++) {
            if (values[i] == t) {
                return i;
            }
        }
        __builtin_unreachable();
    }

    constexpr static size_t get(EnumType t) {
        for (size_t i = 0; i < size; i++) {
            if (values[i] == t) {
                return i;
            }
        }
        // TODO: need a better error
        return static_cast<size_t>(-1);
    }
};

template <typename T, typename Cont>
constexpr size_t find_index(const Cont& xs, const T& x) {
    for (size_t i = 0; i < xs.size(); i++) {
        if (xs[i] == x) {
            return i;
        }
    }
    return static_cast<size_t>(-1);
}


template <typename Indexer, typename ValueType>
class EnumTableEntry {
    using EnumType = typename Indexer::EnumType;
    constexpr static size_t N = Indexer::size;
public:
    template <typename... Args>
    constexpr EnumTableEntry(const Args&... args)
        : values_({args...})
    {}

    // constexpr EnumTableEntry(const std::array<ValueType, N>& values)
    //     : values_(values)
    // {}

    template <EnumType t>
    constexpr const ValueType& get() const {
        return values_[Indexer::template get<t>()];
    }

    constexpr const ValueType& operator[](EnumType t) const {
        return values_[Indexer::get(t)];
    }

    constexpr static auto make_sorted(const std::array<EnumType, N>& enums, const std::array<ValueType, N>& values) {
        return make_sorted_helper(enums, values, std::make_index_sequence<N>{});
    }

private:

    template <size_t... Is>
    constexpr static auto make_sorted_helper(
            const std::array<EnumType, N>& enums,
            const std::array<ValueType, N>& values,
            std::index_sequence<Is...>) {
        return EnumTableEntry<Indexer, ValueType>(values[find_index(enums, Indexer::values[Is])]...);
    }

    std::array<ValueType, N> values_;
};

template <typename Indexer, typename... ValueTypes>
class EnumTable {
    using EnumType = typename Indexer::EnumType;
    using TableTuple = std::tuple<EnumTableEntry<Indexer, ValueTypes>...>;
public:

    template <typename... EntryTypes>
    constexpr static auto make_table(const EntryTypes&... entries) {
        const std::array<EnumType, Indexer::size> enums{std::get<0>(entries)...};
        return EnumTable<Indexer, ValueTypes...>(build_tuple<0>(enums, entries...));
    }

    template <size_t i>
    constexpr auto get() const {
        return std::get<i>(entries_);
    }

    constexpr static size_t num_entries() {
        return Indexer::size;
    }

    constexpr static size_t num_fields() {
        return sizeof...(ValueTypes);
    }

private:
    constexpr EnumTable(const TableTuple& t)
        : entries_(t)
    {}

    template <size_t i, typename... EntryTypes,
             std::enable_if_t<i >= sizeof...(ValueTypes), bool> = true>
    constexpr static auto build_tuple(const std::array<EnumType, Indexer::size>&, const EntryTypes&...) {
        return std::make_tuple<>();
    }

    template <size_t i, typename... EntryTypes,
             std::enable_if_t<i < sizeof...(ValueTypes), bool> = true>
    constexpr static auto build_tuple(const std::array<EnumType, Indexer::size>& enums, const EntryTypes&... entries) {
        using ValueType = typename std::tuple_element<i, std::tuple<ValueTypes...> >::type;
        return std::tuple_cat(
                std::make_tuple(EnumTableEntry<Indexer, ValueType>::make_sorted(enums, {std::get<i+1>(entries)...})),
                build_tuple<i+1>(enums, entries...)
            );
    }

    TableTuple entries_;
};
