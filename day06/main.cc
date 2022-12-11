#include "fast_io.h"
#include "../lib/utils.hh"

constexpr auto example = R"(mjqjpqmgbljsphdztnvjfqwrcgsmlb)"sv;


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