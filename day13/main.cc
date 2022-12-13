#include "../lib/utils.hh"
#include <cassert>
#include <chrono>
#include <concepts>
#include <simdjson.h>
#include <utility>
#include <compare>
#include <variant>

constexpr auto example =
    R"([1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9])"sv;

struct value_t {
    std::variant<int64_t, std::vector<value_t>> val;
    
    constexpr auto as_span() const -> std::span<value_t const> {
        if(auto ptr = std::get_if<int64_t>(&val)) {
            return {this, 1};
        }
        
        return std::get<std::vector<value_t>>(val);
    }
    
    auto to_string() const -> std::string {
        auto result = std::string{};
        
        std::visit(overloaded{
                [&](int64_t value) { result += std::to_string(value); },
                [&](std::span<value_t const> value) {
                    result += "[";
                    for(auto const& val : value){
                        result += val.to_string();
                        result += ", ";
                    }
                    result += "]";
                },
        }, val);
        
        return result;
    }

    template<std::convertible_to<decltype(val)> T>
    static value_t from(T&& t) {
        return value_t{.val = std::move(t)};
    }
    
    template<std::convertible_to<decltype(val)> T>
    constexpr auto operator<=>(T value) {
        return *this <=> value_t{.val = std::move(value)};
    }
    
    constexpr auto operator==(value_t const&other) const -> bool = default;

    constexpr auto operator<=>(value_t const&other) const -> std::weak_ordering {
        if(std::holds_alternative<int64_t>(val) && std::holds_alternative<int64_t>(other.val))
        {
            return std::get<int64_t>(val) <=> std::get<int64_t>(other.val);
        }
        else {
            auto a = as_span();
            auto b = other.as_span();

            for(auto i = 0; i < a.size(); ++i)  {
                if(i >= b.size()) {
                    return std::weak_ordering::greater;
                }
                
                auto const& a_val = a[i];
                auto const& b_val = b[i];
                
                auto comp = a_val <=> b_val;
                
                if(comp != std::weak_ordering::equivalent) {
                    return comp;
                }
            }
            if(a.size() < b.size()) {
                return std::weak_ordering::less;
            }
            
            return std::weak_ordering::equivalent;
        }
    }


    static value_t from_string(std::string_view str) {

        auto parser = simdjson::ondemand::parser{};
        auto json = simdjson::padded_string{str};

        simdjson::ondemand::document doc;
        auto ec = parser.iterate(json).get(doc);

        if (ec)
            throw "nope";

        return from_json_value(doc.get_value().take_value());
    }

    static value_t from_json_value(simdjson::ondemand::value val) {

        switch (val.type()) {
        case simdjson::fallback::ondemand::json_type::array: {
            auto vec = std::vector<value_t>{};
            for (auto value : val.get_array()) {
                vec.emplace_back(value_t::from_json_value(value.value()));
            }

            return value_t::from(vec);
        }

        case simdjson::fallback::ondemand::json_type::number: {
            return value_t::from(val.get_int64());
        }
        default:
            assert(false);
        }
    }
};

using parsed_t = std::array<value_t, 2>;

constexpr auto parse(std::string_view input) {
    auto groups = input | vw::split("\n\n"sv) | vw::transform([](auto &&group) {
                      return to_array<2>(group | vw::split("\n"sv) |
                                         vw::transform(value_t::from_string));
                  });

    return groups;
}

auto solve(range_of<parsed_t> auto groups) -> std::pair<int, int> {
    auto part_1 = 0;
    
    auto first_sentinel = 1; // will be after the 
    auto second_sentinel = first_sentinel + 1; // start at one
    
    auto index = 1;
    for(auto &&[left, right] : groups) {

        if (left < right) {
            part_1 += index;
        }
        
        if(2 > left)
            ++first_sentinel;
        if(2 > right)
            ++first_sentinel;
        if(6 > left)
            ++second_sentinel;
        if(6 > right)
            ++second_sentinel;
        
        ++index;
    }
    
    auto part_2 = first_sentinel * second_sentinel;

    return std::make_pair(part_1, part_2);
}

// static_assert(solve_impl<false>(parse(example), 20) == 10605);
// static_assert(solve_impl<true>(parse(example), 10'000) == 2713310158ll); "too
// many iterations for constexpr"


int main() {
    //auto test = "[1,[2,[3,[4,[5,6,7]]]],8,9]"sv;

    auto parsed = parse(example);
    auto solved = solve(parsed);

    benchmark([]() {
        auto input = fast_io::native_file_loader("input");

        auto [p1, p2] = solve(parse(input));

        println(p1);
        println(p2);
    });
}