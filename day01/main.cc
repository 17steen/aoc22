#include <cstddef>
#include <iostream>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <vector>
#include <charconv>
#include <functional>
#include <cassert>

#include "fast_io.h"

#include "../lib/utils.hh"

constexpr auto parse(std::string_view input) {
    auto elves = input 
        | vw::split("\n\n"sv)
        | vw::transform([](auto&& bag) {
                auto foods = bag | vw::split("\n"sv) | vw::transform(to_int);
                return std::reduce(foods.begin(), foods.end());
        });
    return elves;
}

constexpr auto solve(auto&& elves) -> std::pair<int, int> {
    int result[3];
    rg::partial_sort_copy(elves, result, std::greater{});

    return std::make_pair(result[0], std::reduce(rg::begin(result), rg::end(result)));
}

constexpr auto example = R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000)"sv;

static_assert(solve(parse(example)).first == 24'000);
static_assert(solve(parse(example)).second == 45'000);

int main() {

    auto then = std::chrono::high_resolution_clock::now();

    auto input = fast_io::native_file_loader("input");
    
    //auto elves = parse(input);
    
    auto [p1, p2] = solve(parse(input));

    println(p1);
    println(p2);

    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro> (now - then);
    
    println(diff.count(), "us");

}