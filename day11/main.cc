#include <cassert>

#include "../lib/utils.hh"
#include <chrono>
#include <span>
#include <cstdint>
#include <ctre.hpp>



constexpr auto example =
    R"(Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1)"sv;


struct monkey_t {
    int id;
    std::vector<int64_t> items;
    auto(*operation)(int64_t, int64_t)->int64_t;
    std::optional<int64_t> operation_value;
    int64_t divisible_by;
    int when_true;
    int when_false;
    int64_t inspection_count = {};
};

constexpr auto parse_instructions(std::string_view instructions) {

    auto [all, id, items, op, value, div_by, when_true, when_false] =
        ctre::multiline_match<R"(^Monkey (\d+):
  Starting items: (.+)
  Operation: new = old (.) (\d+)
  Test: divisible by (\d+)
    If true: throw to monkey (\d+)
    If false: throw to monkey (\d+)$)">(instructions);

    if (not all) {
        assert(false);
    }
    constexpr auto plus = [](auto a, auto b) { return a + b; };
    constexpr auto times = [](auto a, auto b) { return a * b; };

    return monkey_t{
        .id = to_int(id),
        .items = to_vec<int64_t>(items.to_view() | vw::split(" ,"sv) |
                             vw::transform(to_int)),
        .operation = nullptr,
        .operation_value = to_int(value),
        .divisible_by = static_cast<int64_t>(to_int(div_by)),
        .when_true = to_int(when_true),
        .when_false = to_int(when_false),
    };
}

auto display(std::span<monkey_t const> monkeys) -> void {
    for(auto const m : monkeys) {

        print("Monkey "sv, m.id, ": "sv);
        for(auto const item : m.items) {
            print(item, ", "sv);
        }

        println(""sv);
    }
    println(""sv);
}

constexpr auto
parse_individual(std::span<std::string_view const, 6> instructions) {
    auto res = monkey_t{};

    {
        auto [all, id] = ctre::match<"^Monkey ([0-9]+):$">(instructions[0]);
        res.id = to_int(id);
    }

    {
        auto [all, items] =
            ctre::match<".+Starting items: (.+)">(instructions[1]);

        res.items =
            to_vec<int64_t>(items | vw::split(", "sv) | vw::transform(to_int));
    }

    {
        auto [all, op, val] =
            ctre::match<".+Operation: new = old (.) (old|[0-9]+)">(instructions[2]);


        if(val != "old"sv)
            res.operation_value = to_int(val);

        if (op == "+"sv)
            res.operation = [](auto a, auto b) { return a + b; };
        else if (op == "*"sv)
            res.operation = [](auto a, auto b) { return a * b; };
        else
            throw "not bueno";

    }

    {
        auto [all, test] =
            ctre::match<".+divisible by ([0-9]+)">(instructions[3]);

        res.divisible_by = to_int(test);
    }

    {
        auto [all, monkey] =
            ctre::match<".+throw to monkey ([0-9]+)">(instructions[4]);

        res.when_true = to_int(monkey);
    }

    {
        auto [all, monkey] =
            ctre::match<".+throw to monkey ([0-9]+)">(instructions[5]);

        res.when_false = to_int(monkey);
    }

    return res;
}

constexpr auto parse(std::string_view input) {
    auto res = input | vw::split("\n\n"sv) | vw::transform([](auto &&vw) {
                   return parse_individual(to_array_of<6, std::string_view>(
                       vw | vw::split("\n"sv)));
               });
    return to_vec<monkey_t>(res);
}

template<bool is_part2>
constexpr auto solve_impl(std::vector<monkey_t> monkeys, int rounds) -> uint64_t {
    auto modulo = 1;

    if constexpr (is_part2)
        for(auto const& m : monkeys)
            modulo *= m.divisible_by;

    for(int i = 0; i < rounds; ++i) {
        for (monkey_t &monkey : monkeys) {
            for(auto const item : monkey.items) {
                auto worry_level = item;
                worry_level = monkey.operation(worry_level, monkey.operation_value.value_or(worry_level));

                if constexpr (is_part2) {
                    worry_level %= modulo;
                }
                else {
                    worry_level /= 3;
                }

                monkey.inspection_count += 1;
                
                auto receiver = (worry_level % monkey.divisible_by == 0) ? monkey.when_true : monkey.when_false;
                monkeys[receiver].items.emplace_back(worry_level);
            }

            monkey.items.clear();
        }
    }

    auto array = std::array<monkey_t, 2>{};
    rg::partial_sort_copy(monkeys, array, rg::greater{}, &monkey_t::inspection_count, &monkey_t::inspection_count); 
    
    return array[0].inspection_count * array[1].inspection_count;
}

auto solve(std::vector<monkey_t> monkeys) -> std::pair<uint64_t, uint64_t> {
    
    constexpr auto divide_by_three = [](int64_t n) { return n / 3; };

    auto part_1 = solve_impl<false>(monkeys, 20);
    auto part_2 = solve_impl<true>(std::move(monkeys), 10'000);
    return std::make_pair(part_1, part_2);
}

static_assert(solve_impl<false>(parse(example), 20) == 10605);
//static_assert(solve_impl<true>(parse(example), 10'000) == 2713310158ll); "too many iterations for constexpr"

int main() {

    benchmark([]() {
        auto input = fast_io::native_file_loader("input");

        auto [p1, p2] = solve(parse(input));

        println(p1);
        println(p2);
    });
}