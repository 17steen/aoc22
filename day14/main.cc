#include "../lib/utils.hh"
#include <chrono>
#include <compare>
#include <complex>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <variant>

constexpr auto example =
    R"(498,4 -> 498,6 -> 496,6
503,4 -> 502,4 -> 502,9 -> 494,9)"sv;

struct pt_t {
    int x;
    int y;

    auto operator==(pt_t const &other) const -> bool = default;

    auto operator+(pt_t other) const -> pt_t {
        auto newp = *this;
        newp.x += other.x;
        newp.y += other.y;
        return newp;
    }
};
template <> struct std::hash<pt_t> {
    size_t operator()(pt_t const &pt) const {
        return static_cast<size_t>(pt.x) << 32 & pt.y;
    }
};

enum class state : char {
    ROCK = 0,
    SAND = 1,

};
using grid_t = std::unordered_map<pt_t, state>;

constexpr auto parse(std::string_view input) {
    auto traces =
        input | vw::split("\n"sv) |
        vw::transform(vw::split(" -> "sv) |
                      vw::transform(vw::split(","sv) | vw::transform(to_int)) |
                      vw::transform([](auto &&r) {
                          auto const [x, y] = to_array_of<2, int>(r);
                          return pt_t{.x = x, .y = y};
                      })) |
        vw::transform([](auto &&r) { return to_vec<pt_t>(r); });

    return traces;
}

using grid_t = std::unordered_map<pt_t, state>;

auto lay_ground(grid_t &grid, pt_t sand) {

}

auto lay_sand(grid_t &grid, pt_t sand, int max_y, bool part_2 = false) -> bool {

    auto under = sand + pt_t{0, 1};
    auto under_left = sand + pt_t{-1, 1};
    auto under_right = sand + pt_t{1, 1};

    if (sand.y > max_y) {
        if(not part_2) {
            return false; // done
        }
        
        // lay ground
        grid.try_emplace(under, state::ROCK);
        grid.try_emplace(under_left, state::ROCK);
        grid.try_emplace(under_right, state::ROCK);
    }

    if (not grid.contains(under))
        return lay_sand(grid, under, max_y, part_2);
    if (not grid.contains(under_left))
        return lay_sand(grid, under_left, max_y, part_2);
    if (not grid.contains(under_right))
        return lay_sand(grid, under_right, max_y, part_2);

    return grid.try_emplace(sand, state::SAND)
        .second; // if we could "lay" the sand, return true;
}

constexpr auto solve(range_of<std::vector<pt_t>> auto traces)
    -> std::pair<int, int> {
    auto grid = grid_t{};

    auto grid_max_x = 0;
    auto grid_max_y = 0;

    for (auto &&trace : traces) {
        for (int i = 1; i < trace.size(); ++i) {
            auto prev = trace[i - 1];
            auto next = trace[i];

            auto [min_x, max_x] = std::minmax({prev.x, next.x});
            auto [min_y, max_y] = std::minmax({prev.y, next.y});

            grid_max_x = std::max({max_x, grid_max_x});
            grid_max_y = std::max({max_y, grid_max_y});

            for (auto x = min_x; x <= max_x; ++x) {
                for (auto y = min_y; y <= max_y; ++y) {
                    grid.try_emplace(pt_t{x, y}, state::ROCK);
                }
            }
        }
    }

    auto source = pt_t{.x = 500, .y = 0};
    
    auto count = 0;

    while (lay_sand(grid, source, grid_max_y))
        ++count;
    
    auto part_1 = count;
    
    while(lay_sand(grid, source, grid_max_y, true)) 
        ++count;
    
    auto part_2 = count;

    return std::make_pair(part_1, part_2);
}

// variant isn't constexpr it seems
// static_assert(solve(parse(example)).first == 13);

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