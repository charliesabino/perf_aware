#include "generator.hpp"
#include "haversine.hpp"
#include "parser.hpp"
#include "profile.hpp"
#include "scanner.hpp"
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <vector>

auto compute(const auto &points) -> double {
  double sum{};

  TimeBandwidth(points.size() * sizeof(double) * 4 + points.size() * 8);

  for (auto &v : points) {
    auto obj = std::get<JsonObject>(v);
    std::vector<double> coords(4);
    for (auto &[k, v] : obj) {
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
  begin_profile();
  uint32_t num_points = atoi(argv[1]);
  std::cout << "# Points: " << num_points << std::endl;

  auto path = gen_data(num_points);
  Scanner s(path);
  auto tokens = s.scan();

  auto obj = parse(tokens);

  auto sum = compute(std::get<std::vector<JsonValue>>(obj["points"]));

  std::cout << "Computed Average Sum: " << std::setprecision(12)
            << (sum / num_points) << std::endl;

  end_and_print_profile();
}
