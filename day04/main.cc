#include <algorithm>
#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include "fast_io.h"

using namespace std::literals;
namespace rg = std::ranges;
namespace vw = rg::views;

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

[[gnu::pure]] constexpr inline int to_int(std::string_view input) {
    if constexpr (std::is_constant_evaluated()) {
        return to_int_impl_slow(input);
    }
    return to_int_impl(input);
}

template <typename Range, typename Value>
concept range_of =
    rg::range<Range> && std::is_convertible_v<rg::range_value_t<Range>, Value>;

constexpr auto example = R"(2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8)"sv;

struct range_t {
    int start;
    int end;

    constexpr auto contains(range_t other) const -> bool {
        return start <= other.start && end >= other.end;
    }

    constexpr auto contains(int n) const -> bool {
        return start <= n && n <= end;
    }
};

template <typename T>
concept Parsed = range_of<T, std::array<range_t, 2>>;

constexpr auto parse(range_of<char> auto &&input) -> Parsed auto {

    constexpr auto take_2 = [](rg::range auto &&rg) {
        auto it = rg.begin();
        return std::to_array({*it, *++it});
    };

    constexpr auto to_range = [](std::array<int, 2> arr) {
        return range_t{.start = arr[0], .end = arr[1]};
    };

    return input | vw::split("\n"sv) |
           vw::transform(
               vw::split(","sv) |
               vw::transform(vw::split("-"sv) | vw::transform(to_int)) |
               vw::transform(take_2) | vw::transform(to_range)) |
           vw::transform(take_2);
}

constexpr auto solve(Parsed auto &&lines) {
    auto part_1 = 0;
    auto part_2 = 0;

    for (auto &&[fst, snd] : lines) {
        if (fst.contains(snd) or snd.contains(fst)) {
            part_1 += 1;
        }
        if (fst.contains(snd.start) or fst.contains(snd.end) or
            snd.contains(fst.start) or snd.contains(fst.end)) {
            part_2 += 1;
        }
    }

    return std::pair{part_1, part_2};
}

static_assert(solve(parse(example)).first == 2);
static_assert(solve(parse(example)).second == 4);

int main() {

    auto then = std::chrono::high_resolution_clock::now();

    auto input = fast_io::native_file_loader("input");

    auto [p1, p2] = solve(parse(input));

    println(p1);
    println(p2);

    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);

    println(diff.count(), "µs");
}