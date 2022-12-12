#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <numeric>
#include <ranges>
#include <string_view>
#include <vector>

#include "fast_io.h"

using namespace std::literals;
namespace rg = std::ranges;
namespace vw = std::ranges::views;

template <size_t N> constexpr inline auto to_array(rg::range auto &&range) {
    auto arr = std::array<rg::range_value_t<decltype(range)>, N>{};


    rg::copy_n(rg::begin(range), N, rg::begin(arr));

    return arr;
}

template <size_t N, typename T> constexpr inline auto to_array_of(rg::range auto &&range) {
    std::array<T, N> arr;
    auto it = rg::begin(range);
    for(size_t i = 0; i < N; ++i) {
        rg::construct_at(std::addressof(arr[i]), *it);
        ++it;
    }
    return arr;
}

template <typename DestType> constexpr auto to_vec(rg::range auto &&range) {
    using value_type = rg::range_value_t<decltype(range)>;
    auto vec = std::vector<DestType>{};
    for (auto &&val : range) {
        vec.emplace_back(std::forward<value_type>(val));
    }
    return vec;
}

[[gnu::pure]] inline int to_int_impl(std::string_view input) {
    int res = -1;
    std::from_chars(input.data(), input.data() + input.size(), res);
    return res;
}

[[gnu::pure]] constexpr inline int to_int_impl_slow(std::string_view input) {

    if (input.empty()) {
        return std::numeric_limits<int>::min();
    }

    auto it = input.begin();

    int sign = 1;
    int result = 0;

    if (*it == '-') {
        sign = -1;
        ++it;
    } else if (*it == '+') {
        ++it;
    }

    for (; it != input.end(); ++it) {
        auto c = *it;
        if (c < '0' || c > '9') {
            return std::numeric_limits<int>::min();
        }
        result *= 10;
        result += c - '0';
    }

    return result * sign;
}

[[gnu::pure]] constexpr int to_int(std::string_view input) {
    if (std::is_constant_evaluated()) {
        return to_int_impl_slow(input);
    }
    return to_int_impl(input);
}

template <typename Range, typename Value>
concept range_of =
    rg::range<Range> && std::is_convertible_v<rg::range_value_t<Range>, Value>;


// helpful with std::visit
template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

auto benchmark(auto&& func) {

    auto then = std::chrono::high_resolution_clock::now();

    func();

    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);

    println(diff.count(), "us");
}