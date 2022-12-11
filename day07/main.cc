#include <variant>

#include "../lib/utils.hh"
#include "fast_io.h"

constexpr auto example = R"($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k)"sv;

constexpr auto parse(std::string_view input)
    -> range_of<std::string_view> auto {
    return input | vw::split("\n"sv);
}

struct node_t;

struct file_node_t {
    size_t size;
};

struct dir_node_t {
    dir_node_t *parent;
    size_t cached_size;
    std::vector<std::pair<std::string_view, node_t>> children;

    [[gnu::pure]] constexpr auto size() const -> size_t;

    constexpr auto for_each(
        std::invocable<std::string_view, file_node_t const &> auto &&on_file,
        std::invocable<std::string_view, dir_node_t const &> auto &&on_dir,
        bool recurse = false) const -> void;
};

struct node_t {
    std::variant<dir_node_t, file_node_t> node;
};

constexpr auto dir_node_t::for_each(
    std::invocable<std::string_view, file_node_t const &> auto &&on_file,
    std::invocable<std::string_view, dir_node_t const &> auto &&on_dir,
    bool recurse) const -> void {
    for (auto const &[name, node] : children) {
        if (auto const *dir = std::get_if<dir_node_t>(&node.node); dir) {
            on_dir(name, *dir);
            if (recurse) {
                dir->for_each(on_file, on_dir, true);
            }
        } else if (auto const *file = std::get_if<file_node_t>(&node.node);
                   file) {
            on_file(name, *file);
        }
    }
}

[[gnu::pure]] constexpr auto dir_node_t::size() const -> size_t {
    auto size = size_t{0};

    for_each([&](std::string_view name,
                 file_node_t const &file) { size += file.size; },
             [&](std::string_view name, dir_node_t const &dir) {
                 auto prev = size;
                 size += dir.size();
             });

    return size;
}


auto tree(dir_node_t const &root, int indent = 0) -> void {
    if (indent == 0)
        println(". "sv, root.size());

    for (auto const &[name, node] : root.children) {
        for (int i = 0; i < indent; ++i) {
            print(" "sv);
        }
        print("|_", name);

        std::visit(overloaded{[](file_node_t const &file) {
                                  println(" size: "sv, file.size);
                              },
                              [=](dir_node_t const &dir) {
                                  println(" size: "sv, dir.size());
                                  tree(dir, indent + 1);
                              }},
                   node.node);
    }
}

constexpr auto solve(range_of<std::string_view> auto &&input) -> std::pair<int, int> {

    auto part_1 = 0;

    auto root_node = dir_node_t{};

    auto curr_dir = &root_node;

    for (std::string_view const line : input) {
        if (line[0] == '$') { // parse command
            auto [_, cmd, arg] = to_array<3>(line | vw::split(" "sv));
            if (cmd == "cd"sv) {
                [[unlikely]] if (arg == "/"sv) {
                    curr_dir = &root_node;
                } else if (arg == ".."sv) {
                    curr_dir = curr_dir->parent ?: curr_dir;
                } else {
                    auto node_it  = rg::find_if(curr_dir->children, [path= std::string_view{arg}](const auto& pair) {
                            return pair.first == path;
                    });

                    if(node_it != rg::end(curr_dir->children)) {

                    }
                    curr_dir = &std::get<dir_node_t>(node_it->second.node);
                }
            } else if (cmd == "ls"sv) {
                // this effectively does nothing
            }

        } else { // result
            auto const [fst, snd] = to_array<2>(line | vw::split(" "sv));

            auto const name = std::string_view{snd};

            if (fst == "dir"sv) {
                auto node = node_t {
                    .node = dir_node_t{.parent = curr_dir},
                };

                curr_dir->children.emplace_back(std::make_pair(name, node));
            } else {
                auto size = (size_t)to_int(fst);
                curr_dir->children.emplace_back(std::make_pair(
                    name, node_t{.node = file_node_t{.size = size}}));
                auto dir = curr_dir;
                while (dir) {
                    dir->cached_size += size;
                    dir = dir->parent;
                }
            }
        }
    }

    //if(!std::is_constant_evaluated()) 
    //    println("root size: "sv, root_node.cached_size);

    constexpr auto fs_size = size_t{70'000'000};
    constexpr auto needed_space = size_t{30'000'000};

    auto const used_space = root_node.size();
    auto const to_be_freed = needed_space - (fs_size - used_space);

    auto smallest_to_remove = used_space;
    root_node.for_each([](std::string_view, file_node_t const &) {},
                       [=, &part_1, &smallest_to_remove](

                           std::string_view name, dir_node_t const &dir) {
                           auto const size = dir.size();

                           if (size <= 100'000) {
                               part_1 += size;
                           }
                           if (size >= to_be_freed) {
                               smallest_to_remove =
                                   std::min(size, smallest_to_remove);
                           }
                       },
                       true);

    return std::pair{part_1, smallest_to_remove};
}

int main() {

    static_assert(solve(parse(example)).first == 95437);
    static_assert(solve(parse(example)).second == 24933642);

    auto then = std::chrono::high_resolution_clock::now();

    auto input = fast_io::native_file_loader("input");

    auto [p1, p2] = solve(parse(input));

    println(p1);
    println(p2);

    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);

    println(diff.count(), "µs");
}