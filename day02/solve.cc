#include <algorithm>
#include <array>
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
#include <vector>
#include <cassert>

#include "fast_io.h"

using namespace std::literals;
namespace rg = std::ranges;
namespace vw = rg::views;

[[gnu::pure]] inline int to_int(std::string_view input) {
    int res = -1;
    std::from_chars(input.data(), input.data() + input.size(), res);
    return res;
}

constexpr auto to_vec(auto &&view) {
    using value_type = std::decay_t<decltype(*view.begin())>;
    auto result = std::vector<value_type>{};
    std::copy(view.begin(), view.end(), std::back_inserter(result));

    return result;
}

enum class Hand : int {
    Rock = 0,
    Paper = 1,
    Scisors = 2,
};

constexpr auto ch_to_hand(char ch) -> Hand {
    if (ch == 'A')
        return Hand::Rock;
    if (ch == 'B')
        return Hand::Paper;
    if (ch == 'C')
        return Hand::Scisors;
    return Hand(-1);
}

constexpr auto parse(std::string_view input) {
    return input | vw::split("\n"sv) | vw::transform([](auto &&it) {
               return std::pair{ch_to_hand(it[0]), it[2]};
           });

}


constexpr Hand operator+(Hand h) { return Hand((int(h) + 1) % 3); }
constexpr Hand operator-(Hand h) { return Hand((int(h) + 2) % 3); }

constexpr std::strong_ordering operator<=>(Hand a, Hand b) {
    if (a == b) {
        return std::strong_ordering::equal;
    }
    if (+a == b) {
        return std::strong_ordering::less;
    }

    return std::strong_ordering::greater;
}

static_assert(Hand::Scisors > Hand::Paper);
static_assert(Hand::Paper > Hand::Rock);
static_assert(Hand::Rock > Hand::Scisors);

constexpr auto to_score(Hand p1, Hand p2) -> int {
    int score = int(p2) + 1;

    if (p2 > p1) {
        score += 6;
    } else if (p2 == p1) {
        score += 3;
    }
    return score;
}

constexpr auto as_part2(Hand p1, char action) -> Hand {
    if (action == 'X') {
        return -p1;
    }
    if (action == 'Y') {
        return p1;
    }
    if (action == 'Z') {
        return +p1;
    }

    return Hand(-1);
}

constexpr auto solve(auto &&input) {
    int score_part1 = 0;
    int score_part2 = 0;

    for (auto const [p1, ch] : input) {
        auto const part1 = Hand(ch - 'X');
        auto const part2 = as_part2(p1, ch);

        score_part1 += to_score(p1, part1);
        score_part2 += to_score(p1, part2);
    }

    return std::pair{score_part1, score_part2};
}

constexpr auto example = R"(A Y
B X
C Z)"sv;

static_assert(solve(parse(example)).first == 15);
static_assert(solve(parse(example)).second == 12);

auto solve_fast(const char *begin, const char *const end) {
    auto part_1 = 0;
    auto part_2 = 0;

    // clang-format off
    char const score[3][3]{
        {3, 0, 6}, 
        {6, 3, 0}, 
        {0, 6, 3}
    };
    char const should_play[3][3] {
        {2, 0, 1},
        {0, 1, 2},
        {1, 2, 0},
    };
    // clang-format on

    for (; begin < end; begin += 4) {
        auto const p1 = begin[0] - 'A';
        auto const p2 = begin[2] - 'X';

        part_1 += p2 + 1 + score[p2][p1];
        part_2 += should_play[p2][p1] + 1 + p2 * 3;
    }

    return std::pair{part_1, part_2};
}

//static_assert(solve_fast(example.data(), example.data() + example.size()).first == 15);
//static_assert(solve_fast(example.data(), example.data() + example.size()).second == 12);

int main() {
    auto then = std::chrono::high_resolution_clock::now();

    auto input = fast_io::native_file_loader("input");

    auto [p1, p2] = solve_fast(input.address_begin, input.address_end);

    println(p1);
    println(p2);

    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);

    println(diff.count(), "us");
}
