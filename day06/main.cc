#include <algorithm>
#include <bitset>
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

constexpr auto example = R"(mjqjpqmgbljsphdztnvjfqwrcgsmlb)"sv;

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


constexpr auto parse(range_of<char> auto &&input) {
    return std::string_view{input};
}

constexpr auto is_marker(std::string_view str) -> bool {
    auto bitset = std::array<bool, 'z' - 'a' + 1>{};
    for(auto const ch : str) {
        if(bitset[ch - 'a'] == true) {
            return false;
        }
        bitset[ch - 'a'] = true;
    }

    return true;
}


constexpr auto solve(std::string_view input) -> std::pair<int, int> {
    constexpr auto window_size_p1 = 4;
    constexpr auto window_size_p2 = 14;

    auto part_1 = std::optional<int>{};
    auto part_2 = std::optional<int>{};
    
    for(int i = 0; i < input.size() - window_size_p2; ++i) {

        auto start_if_marker = [=](auto window_size) -> std::optional<int> {
            if(is_marker(input.substr(i, window_size)))
                return i + window_size;
            return {};
        };

        if(!part_1){
            part_1 = start_if_marker(window_size_p1);
            // just in case they share the same window
            part_2 = start_if_marker(window_size_p2); 
        }
        else if(!part_2) {
            part_2 = start_if_marker(window_size_p2);
        }
        else { // solved
            break;
        }
    }

    return std::pair{*part_1, *part_2};
}

static_assert(solve(parse(example)).first == 7);
static_assert(solve(parse(example)).second == 19);

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