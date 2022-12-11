#include <algorithm>
#include <iostream>
#include <bitset>
#include <cassert>
#include <charconv>
#include <chrono>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "fast_io.h"

using namespace std::literals;
namespace rg = std::ranges;
namespace vw = rg::views;

template <typename Range, typename Value>
concept range_of =
    rg::range<Range> && std::is_convertible_v<rg::range_value_t<Range>, Value>;

constexpr auto example = 
R"(30373
25512
65332
33549
35390)"sv;

template <typename DestType> constexpr auto to_vec(rg::range auto &&range) {
    using value_type = rg::range_value_t<decltype(range)>;
    auto vec = std::vector<DestType>{};
    for (auto &&val : range) {
        vec.emplace_back(std::forward<value_type>(val));
    }
    return vec;
}

constexpr auto parse(std::string_view input)
    -> range_of<std::string_view> auto {
    return to_vec<std::string_view>(input | vw::split("\n"sv));
}

template <size_t N> constexpr inline auto to_array(rg::range auto &&range) {
    auto arr = std::array<rg::range_value_t<decltype(range)>, N>{};

    rg::copy_n(rg::begin(range), N, rg::begin(arr));

    return arr;
}

constexpr auto scenic_score(std::span<std::string_view const> trees, int i, int j) -> std::pair<int, bool> {
    auto const directions = {
        std::pair{-1, 0}, 
        std::pair{0, -1},
        std::pair{1, 0},
        std::pair{0, 1},
    };

    auto score = 1;
    
    auto is_visible_from_outside = false;
    

    for(auto const [v, h] : directions) {
        int new_i = i;
        int new_j = j;
        
        auto curr_height = trees[i][j];
        
        int count = 0;

        while(true) {
            new_i += v;
            new_j += h;
            
            if(new_i < 0 || new_j < 0 || new_i >= trees.size() || new_j >= trees[0].size())  {
                is_visible_from_outside = true;
                break;
            }
            
            auto next_tree = trees[new_i][new_j];

            ++count;
            
            if(curr_height <= next_tree) {
                break;
            }
        }
        score *= count;
    }

    return {score, is_visible_from_outside};
}

constexpr auto solve(std::span<std::string_view const> trees) -> std::pair<int, int> {

    auto part_1 = 0;
    auto part_2 = 0;

    auto rows = trees.size();
    auto columns = trees.front().size();

    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < columns; ++j) {
            auto const [score, visible] = scenic_score(trees, i, j);

            if(visible) {
                ++part_1;
            }
            
            part_2 = std::max(score, part_2);
        }
    }
    
    return std::pair{part_1, part_2};
}

static_assert(solve(parse(example)).first == 21);
static_assert(solve(parse(example)).second == 8 );

int main() {

    assert(solve(parse(example)).first ==21);
    assert(solve(parse(example)).second == 8);

    auto then = std::chrono::high_resolution_clock::now();

    auto input = fast_io::native_file_loader("input");

    auto [p1, p2] = solve(parse(input));

    println(p1);
    println(p2);

    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);

    println(diff.count(), "µs");
}