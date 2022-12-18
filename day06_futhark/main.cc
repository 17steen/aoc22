#include "lib.h"
#include "fast_io.h"
#include <cstddef>
#include <vector>
#include <chrono>
#include <span>
#include <string_view>
#include <cstdint>

using namespace std::string_view_literals;

int main() {
    // i won't count the initialization as part of the runtime, although it sure is
    auto *cfg = futhark_context_config_new();
    futhark_context_config_set_cache_file(cfg, "futhark.cache");
    auto* context = futhark_context_new(cfg);

    auto then = std::chrono::high_resolution_clock::now();

    auto file = fast_io::native_file_loader{"input"};
    auto* arr = futhark_new_u8_1d(context, reinterpret_cast<uint8_t*>(file.data()), file.size());



    long part1;
    long part2;
    futhark_entry_solve(context, &part1, &part2, arr);
    
    println(part1);
    println(part2);
    auto now = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double, std::micro>(now - then);
    println(diff.count(), "µs");
    
    futhark_free_u8_1d(context, arr);
    futhark_context_free(context);
    futhark_context_config_free(cfg);
}
