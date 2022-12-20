#include "../lib/utils.hh"
#include <cassert>
#include <chrono>
#include <numeric>
#include <cstdint>
#include <ranges>
#include <unordered_set>

constexpr auto example =
    R"(1
2
-3
3
-2
0
4)"sv;

constexpr auto parse(std::string_view input) -> auto {
    return input | vw::split("\n"sv) | vw::transform([i = 0](auto a) mutable {
               return std::make_pair(i++, static_cast<int64_t>(to_int(a)));
           });
}

constexpr auto mod(int64_t n, int64_t max) {
    auto res = n % max;

    if (res < 0) {
        return max + res;
    }
    return res;
}

constexpr auto solve(auto input) -> std::pair<int, int> {
    // pairs of their original index and the value
    auto original = to_vec<std::pair<int, int64_t>>(input);
    
    auto mix = [&](auto& original) {
        for (int i = 0; i < original.size(); ++i) {
            auto it = rg::find(original, i, &std::pair<int, int64_t>::first);
            auto idx = it - original.begin();

            auto move = it->second;

            if (move == 0) {
                continue;
            }

            auto cpy = *it;
            original.erase(it);

            auto new_idx = mod(move + idx, original.size());

            original.insert(original.begin() + new_idx, cpy);
        }
    };


    auto get_grove = [](auto const &arr) {
        auto size = arr.size();
        auto idx_of_zero =
            rg::find(arr, 0, &std::pair<int, int64_t>::second) - arr.begin();

        return arr[mod(idx_of_zero + 1000, size)].second +
               arr[mod(idx_of_zero + 2000, size)].second +
               arr[mod(idx_of_zero + 3000, size)].second;
    };
    
    auto copy = original;
    mix(copy);
    auto part_1 = get_grove(copy);
    
    for(auto& [idx, value] : original) {
        value *= 811589153;
    }
    
    for(int i = 0; i < 10; ++i){
        mix(original);
    }
    

    auto part_2 = get_grove(original);
    return std::make_pair(part_1, part_2);
}

constexpr auto example_2 = R"(0
-5
1)";

static_assert(solve(parse(example)).first == 3);
static_assert(solve(parse(example)).second == 1623178306);

static_assert(solve(parse(example_2)).first == -4);
//static_assert(solve(parse(example_2)).second == -3246356612); fails

int main() {

    auto parsed = parse(example);
    auto [p1, p2] = solve(parsed);

    println(p1);
    println(p2);

    benchmark([]() {
        auto input = fast_io::native_file_loader("input");

        auto [p1, p2] = solve(parse(input));

        println(p1);
        println(p2);
    });
}