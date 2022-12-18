#include "lib.h"
#include "fast_io.h"
#include <vector>
#include <chrono>
#include <span>
#include <string_view>
#include <cstdint>

using namespace std::string_view_literals;

int main() {
    // i won't count the initialization as part of the runtime, although it sure is
    auto *cfg = futhark_context_config_new();
    auto* context = futhark_context_new(cfg);

    auto then = std::chrono::high_resolution_clock::now();

    auto file = fast_io::native_file_loader{"input"};
    auto* arr = futhark_new_i8_1d(context, reinterpret_cast<int8_t*>(file.data()), file.size());


    long part1;
    futhark_entry_solve(context, &part1, 4, arr);
    long part2;
    futhark_entry_solve(context, &part2, 14, arr);
    
    println(part1);
    println(part2);
    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);
    println(diff.count(), "µs");
    
    futhark_free_i8_1d(context, arr);
    futhark_context_free(context);
    futhark_context_config_free(cfg);
}
