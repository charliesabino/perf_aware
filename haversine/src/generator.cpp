#include "haversine.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <random>

auto get_rand(std::mt19937 &gen, double min, double max) -> double {
  std::uniform_real_distribution<double> dis(min, max);
  return dis(gen);
}

auto gen_data(int32_t num_points) -> std::string {
  double sum{};
  std::random_device rd;
  std::mt19937 gen(rd());
  std::filesystem::create_directories("data/");
  auto path = std::format("data/haversine{}.json", num_points);
  std::ofstream f(path);
  f << "{\"points\": [";

  while (num_points--) {
    auto x0 = get_rand(gen, -180, 180);
    auto x1 = get_rand(gen, -180, 180);
    auto y0 = get_rand(gen, -90, 90);
    auto y1 = get_rand(gen, -90, 90);
    sum += haversine(x0, y0, x1, y1, EARTH_RADIUS);

    if (num_points > 0) {
      f << std::format("{{\"x0\": {}, \"y0\": {}, \"x1\": {}, \"y1\": {}}},",
                       x0, y0, x1, y1);
    } else {
      f << std::format("{{\"x0\": {}, \"y0\": {}, \"x1\": {}, \"y1\": {}}}", x0,
                       y0, x1, y1);
    }
  }
  f << "]}" << std::flush;
  f.close(); // Explicitly close the file to ensure all data is written

  std::cout << "Real Sum: " << std::fixed << std::setprecision(16) << sum
            << std::endl;

  return path;
}
