#include "generator.hpp"
#include "haversine.hpp"
#include "parser.hpp"
#include "profile.hpp"
#include "scanner.hpp"
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
  begin_profile();
  auto num_points = atoi(argv[1]);
  std::cout << "# Points: " << num_points << std::endl;

  auto path = gen_data(num_points);
  Scanner s(path);
  auto tokens = s.scan();

  auto obj = parse(tokens);

  auto sum = compute(obj);

  std::cout << "Computed Sum: " << std::fixed << std::setprecision(16) << sum
            << std::endl;

  end_and_print_profile();
}
