#include <algorithm>
#include <bits/ranges_algo.h>
#include <bits/ranges_uninitialized.h>
#include <cassert>
#include <charconv>
#include <unordered_set>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "fast_io.h"

using namespace std::literals;
namespace rg = std::ranges;
namespace vw = std::ranges::views;

[[gnu::pure]] inline int to_int(std::string_view input) {
    int res = -1;
    std::from_chars(input.data(), input.data() + input.size(), res);
    return res;
}

constexpr uint uncertain_parse(std::string_view input) {
    auto res = 0u;
    for (auto const c : input) {
        res *= 10;
        res += c & 0xFu;
    }

    return res;
}

constexpr auto example = R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)"sv;

constexpr auto example2 = R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)"sv;

constexpr auto parse(auto &&input) { return input | vw::split("\n"sv); }

constexpr auto split_in_2(std::string_view line) {
    auto const half = rg::size(line) / 2;
    return std::pair{line.substr(0, half), line.substr(half)};
}

constexpr auto to_priority(char ch) -> int {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 1;
    } else if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 26 + 1;
    }

    // shouldn't happen
    return 0;
}


template<int N>
constexpr auto part2(std::span<std::string_view, N> chunk) {
    bool maps[chunk.size()][26 * 2 + 1]{};

    for (int i = 0; i < chunk.size(); ++i) {
        for (auto const idx : chunk[i] | vw::transform(to_priority)) {
            maps[i][idx] |= true;
        }
    }

    for (int i = 0; i < sizeof(*maps); ++i) {
        auto contained = true;
        for(auto const& map : maps) {
            contained &= map[i];
        }
        if(contained) return i;
    }

    return 0;
}

constexpr auto solve(auto &&lines) {
    auto part_1 = 0;
    auto part_2 = 0;

    constexpr auto group_size = 3;
    std::array<std::string_view, group_size> chunk;
    int i = 0;

    for (auto const line : lines) {
        chunk[i] = line;
        ++i;
        if (i == group_size) { // chunk
            part_2 += part2<group_size>(chunk);
            i = 0;
        }

        auto [fst, snd] = split_in_2(line);

        auto it =
            rg::find_if(fst, [s = snd](auto ch) { return s.contains(ch); });

        if (it != rg::end(fst)) {
            part_1 += to_priority(*it);
        }
    }

    // otherwise handle
    assert(i == 0);

    return std::pair{part_1, part_2};
}

static_assert(solve(parse(example)).first == 157);
static_assert(solve(parse(example2)).second == 70);

constexpr auto to_vec(auto &&view) {
    using value_type = std::decay_t<decltype(*view.begin())>;
    auto result = std::vector<value_type>{};
    std::copy(view.begin(), view.end(), std::back_inserter(result));

    return result;
}

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