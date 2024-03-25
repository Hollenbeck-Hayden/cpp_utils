#pragma once

#include <type_traits>
#include <array>
#include <cstddef>
#include <tuple>
#include <utility>
#include <optional>

namespace {

template <typename T, typename Cont>
constexpr std::optional<size_t> find_index(const Cont& xs, const T& x) {
    for (size_t i = 0; i < xs.size(); i++) {
        if (xs[i] == x) {
            return i;
        }
    }
    return std::nullopt;
}

template <typename T, typename Cont>
constexpr bool contains(const Cont& xs, const T& x) {
    for (size_t i = 0; i < xs.size(); i++) {
        if (xs[i] == x) {
            return true;
        }
    }
    return false;
}

template <typename CommonType>
constexpr std::optional<CommonType> any_optional() {
    return std::nullopt;
}

template <typename CommonType, typename... ValueTypes>
constexpr std::optional<CommonType> any_optional(const std::optional<CommonType>& value, ValueTypes... values) {
    if (value) return value;
    return any_optional<CommonType>(values...);
}



}


template <typename Enum, Enum... EnumValues>
class EnumIndexer {
public:
    using EnumType = Enum;
    constexpr static size_t size = sizeof...(EnumValues);
    constexpr static std::array<Enum, size> values{EnumValues...};

    template <EnumType t>
    constexpr static bool contains() {
        return ::contains(values, t);
    }

    template <EnumType t>
    constexpr static size_t get() {
        static_assert(contains<t>());
        return *::find_index(values, t);
    }

    constexpr static std::optional<size_t> get(EnumType t) {
        return ::find_index(values, t);
    }
};


template <typename Indexer, typename ValueType>
class EnumTableEntry {
    using EnumType = typename Indexer::EnumType;
    constexpr static size_t N = Indexer::size;
public:
    template <typename... Args>
    constexpr EnumTableEntry(const Args&... args)
        : values_({args...})
    {}

    template <EnumType t>
    constexpr const ValueType& get() const {
        return values_[Indexer::template get<t>()];
    }

    constexpr const ValueType* operator[](EnumType t) const {
        constexpr auto index = Indexer::get(t);
        if (index) return &values_[index];
        return nullptr;
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
        return EnumTableEntry<Indexer, ValueType>(safe_reverse_lookup(enums, values, Is)...);
    }

    constexpr static auto safe_reverse_lookup(
            const std::array<EnumType, N>& enums,
            const std::array<ValueType, N>& values,
            size_t i)
    {
        auto index = find_index(enums, Indexer::values[i]);
        return values[*index];
    }

    std::array<ValueType, N> values_;
};


template <typename Indexer, typename FieldsIndexer, typename... ValueTypes>
class EnumTable {
    using Self = EnumTable<Indexer, FieldsIndexer, ValueTypes...>;
    using EnumType = typename Indexer::EnumType;
    using FieldEnum = typename FieldsIndexer::EnumType;
    using TableTuple = std::tuple<EnumTableEntry<Indexer, ValueTypes>...>;

    template <FieldEnum field>
    using FieldType = typename std::tuple_element<FieldsIndexer::template get<field>(), std::tuple<ValueTypes...> >::type;

public:

    template <typename... EntryTypes>
    constexpr static auto make_table(const EntryTypes&... entries) {
        static_assert(sizeof...(ValueTypes) == FieldsIndexer::size);
        const std::array<EnumType, Indexer::size> enums{std::get<0>(entries)...};
        return EnumTable<Indexer, FieldsIndexer, ValueTypes...>(build_tuple<0>(enums, entries...));
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

    template <FieldEnum field>
    constexpr auto get() const {
        return get<FieldsIndexer::template get<field>()>();
    }

    template <EnumType i, FieldEnum field>
    constexpr auto get() const {
        return get<field>().template get<i>();
    }

    template <FieldEnum field>
    constexpr auto get(EnumType i) const {
        return get<field>()[i];
    }

    template <FieldEnum field>
    constexpr std::optional<EnumType> lookup(const FieldType<field>& value) const {
        return lookup_helper<field>(value, std::make_index_sequence<Indexer::size>{});
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

    template <FieldEnum field, size_t... Is>
    constexpr std::optional<EnumType> lookup_helper(const FieldType<field>& value, std::index_sequence<Is...>) const {
        return any_optional(lookup_match<Indexer::values[Is], field>(value)...);
    }

    template <EnumType i, FieldEnum field>
    constexpr std::optional<EnumType> lookup_match(const FieldType<field>& value) const {
        if (get<i, field>() == value) return i;
        return std::nullopt;
    }

    TableTuple entries_;
};
