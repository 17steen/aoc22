#include "../lib/utils.hh"
#include <boost/container_hash/hash.hpp>
#include <cassert>
#include <chrono>
#include <ctre.hpp>
#include <numeric>
#include <ranges>
#include <unordered_set>

constexpr auto example =
    R"(2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5)"sv;

struct pt3d_t {
    int x;
    int y;
    int z;

    constexpr auto operator+(pt3d_t const other) const -> pt3d_t {
        return {
            .x = x + other.x,
            .y = y + other.y,
            .z = z + other.z,
        };
    }

    constexpr auto operator+=(pt3d_t const other) -> decltype(auto) {
        return (*this) = (*this) + other;
    }

    constexpr auto operator==(pt3d_t const &) const -> bool = default;

    constexpr auto operator>(pt3d_t const &other) const -> bool {
        return x > other.x || y > other.y || z > other.z;
    }

    constexpr auto operator<(pt3d_t const &other) const -> bool {
        return x < other.x || y < other.y || z < other.z;
    }
};

static constexpr auto directions = {
    pt3d_t{
        1,
        0,
        0,
    },
    {
        0,
        1,
        0,
    },
    {
        0,
        0,
        1,
    },
    pt3d_t{
        -1,
        0,
        0,
    },
    {
        0,
        -1,
        0,
    },
    {
        0,
        0,
        -1,
    },
};

template <> struct std::hash<pt3d_t> {
    auto operator()(pt3d_t pt) const -> size_t {
        auto result = size_t{0};
        boost::hash_combine(result, pt.x);
        boost::hash_combine(result, pt.y);
        boost::hash_combine(result, pt.z);
        return result;
    }
};

constexpr auto parse_one(std::string_view line) -> pt3d_t {
    auto const [x, y, z] =
        to_array<3>(line | vw::split(","sv) | vw::transform(to_int));

    return {x, y, z};
}

constexpr auto parse(std::string_view input) -> range_of<pt3d_t> auto {
    return input | vw::split("\n"sv) | vw::transform(parse_one);
}

auto has_access_to_air(std::unordered_set<pt3d_t> const &points,
                       pt3d_t const pt, pt3d_t const min, pt3d_t const max,
                       std::unordered_set<pt3d_t>& already_visited,
                       std::unordered_map<pt3d_t, bool>& global_cache) -> bool {

    if(points.contains(pt))
        return false;

    // if it has already been visited and the answer was found, we wouldn't be here
    if(not already_visited.emplace(pt).second)
        return global_cache[pt] = false;
    
    if(auto it = global_cache.find(pt); it !=  global_cache.end())
        return it->second;

    for (auto const dir : directions) {
        auto const next_to = pt + dir;


        if (next_to < min or max < next_to) {
            return global_cache[next_to] = global_cache[pt] = true;
        }
        
        if(has_access_to_air(points, next_to, min, max, already_visited, global_cache)) {
            return global_cache[next_to] = global_cache[pt] = true;
        }
    }
    
    return false;
}

auto exposed_sides_both(std::unordered_set<pt3d_t> const &points, pt3d_t const min, pt3d_t const max) -> std::pair<int, int> {

    auto p1 = 0;
    auto p2 = 0;
    
    auto global_cache = std::unordered_map<pt3d_t, bool>{};
    

    for (auto const pt : points) {
        for (auto const dir : directions) {

            auto const next_to = pt + dir;

            auto already_visited = std::unordered_set<pt3d_t>{};
            if (has_access_to_air(points, next_to, min, max, already_visited, global_cache)) {
                p2 += 1;
            }
            
            if(already_visited.size() > 0) { // this means that we had to travel further than the sides, there was thus at least an air bubble
                p1 += 1;
            }
        }
    }

    return {p1, p2};
}

auto solve(range_of<pt3d_t> auto points) -> std::pair<int, int> {
    auto part_1 = 0;
    auto part_2 = 0;

    auto set = std::unordered_set<pt3d_t>{};

    auto min = pt3d_t{
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
    };

    auto max = pt3d_t{
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::min(),
    };

    for (auto const pt : points) {
        set.emplace(pt);

        min.x = std::min(pt.x, min.x);
        min.y = std::min(pt.y, min.y);
        min.z = std::min(pt.z, min.z);

        max.x = std::max(pt.x, max.x);
        max.y = std::max(pt.y, max.y);
        max.z = std::max(pt.z, max.z);
    }

    return exposed_sides_both(set, min, max);
}

constexpr auto my_example = 
R"(1,0,0
0,1,0
0,0,1
-1,0,0
0,-1,0
0,0,-1)"sv;

int main() {

    auto parsed = parse(my_example);
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