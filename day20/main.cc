#include "../lib/utils.hh"
#include <cassert>
#include <chrono>
#include <iostream>
#include <cstdint>
#include <numeric>
#include <ranges>
#include <unordered_set>
#include <gmpxx.h>

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
               return std::make_pair(i++, mpz_class(to_int(a)));
           });
}


constexpr auto abs(std::integral auto a) {
    return a < 0 ? -a : a;
}

constexpr auto mod_should_work(int64_t n, int64_t max) -> size_t {
    auto r = n % max;
    return r >= 0 ? r : r + abs(max);
}

auto mod(int64_t n, size_t max) -> size_t {
    auto big_n  = mpz_class{n};
    
    auto first_result = mod_should_work(n, max);

    //auto result = mpz_class{};
    
    auto result = mpz_fdiv_ui(big_n.get_mpz_t(), max);
    
    //assert(result == first_result);
    
    return result;
}

auto mod(mpz_class const& n, size_t max) -> size_t {
    return mpz_fdiv_ui(n.get_mpz_t(), max);
}

using pair = std::pair<int, mpz_class>;

auto solve(auto input) -> std::pair<long, long> {
    // pairs of their original index and the value
    auto original = to_vec<pair>(input);

    auto mix = [&](auto &original) {
        for (int i = 0; i < original.size(); ++i) {
            auto it = rg::find(original, i, &pair::first);
            auto idx = it - original.begin();

            auto move = it->second;

            if (move == 0) {
                continue;
            }

            auto cpy = *it;
            original.erase(it);

            auto new_idx =
                mod(move + idx, static_cast<int64_t>(original.size()));

            original.insert(original.begin() + static_cast<long>(new_idx), cpy);
        }
    };

    auto get_grove = [](auto const &arr) {
        auto size = arr.size();
        auto idx_of_zero =
            rg::find(arr, 0, &pair::second) - arr.begin();

        return mpz_class{arr[mod(idx_of_zero + 1000, size)].second +
               arr[mod(idx_of_zero + 2000, size)].second +
               arr[mod(idx_of_zero + 3000, size)].second};
    };

    auto copy = original;
    mix(copy);
    auto part_1 = get_grove(copy).get_si();

    for (auto &[idx, value] : original) {
        value *= 811589153;
    }

    for (int i = 0; i < 10; ++i) {
        mix(original);
    }

    auto part_2 = get_grove(original).get_si();


    return std::make_pair(part_1, part_2);
}

constexpr auto example_2 = R"(0
-5
1)";

//static_assert(solve(parse(example)).first == 3);
//static_assert(solve(parse(example)).second == 1623178306);
//
//static_assert(solve(parse(example_2)).first == -4);
//static_assert(solve(parse(example_2)).second == -3246356612); // fails

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