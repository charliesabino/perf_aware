#include "generator.hpp"
#include "haversine.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <sys/_types/_u_int64_t.h>
#include <vector>

auto compute(JsonObject &obj) -> double {
  double sum{};

  auto points = std::get<std::vector<JsonValue>>(obj["points"]);
  for (auto &v : points) {
    auto obj = std::get<JsonObject>(v);
    std::vector<double> coords(4);
    for (auto [k, v] : obj) {
      int i{};
      if (k == "y0")
        i = 1;
      else if (k == "x1")
        i = 2;
      else if (k == "y1")
        i = 3;
      coords[i] = (std::get<double>(v));
    }
    sum += haversine(coords[0], coords[1], coords[2], coords[3], EARTH_RADIUS);
  }
  return sum;
}

int main(int argc, char *argv[]) {
  auto num_points = atoi(argv[1]);
  std::cout << "# Points: " << num_points << std::endl;

  auto measure_time = []<typename F>(std::string_view label, F &&func) {
    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t start_cycles{}, end_cycles{};
    asm volatile("mrs %0, cntvct_el0" : "=r"(start_cycles));

    auto result = std::forward<F>(func)();
    asm volatile("mrs %0, cntvct_el0" : "=r"(end_cycles));
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>{end_time - start_time};
    auto cycles = end_cycles - start_cycles;

    std::cout << label << " Time: " << std::fixed << std::setprecision(8)
              << duration.count() << "s\n";

    std::cout << label << " Cycles: " << cycles << '\n';

    return result;
  };

  auto path =
      measure_time("Generation", [&]() { return gen_data(num_points); });

  auto tokens = measure_time("Scan", [&]() {
    Scanner s(path);
    return s.scan();
  });

  auto obj = measure_time("Parse", [&]() { return parse(tokens); });

  auto sum = measure_time("Compute", [&]() { return compute(obj); });

  std::cout << "Computed Sum: " << std::fixed << std::setprecision(16) << sum
            << std::endl;
}
