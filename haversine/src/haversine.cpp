#include "haversine.hpp"
#include <cmath>

double square(double a) { return a * a; }

double deg_to_rad(double Degrees) { return 0.01745329251994329577 * Degrees; }

// NOTE(casey): EarthRadius is generally expected to be 6372.8
double haversine(double x_0, double y_0, double x_1, double y_1,
                 double radius) {
  /* NOTE(casey): This is not meant to be a "good" way to calculate the
     Haversine distance. Instead, it attempts to follow, as closely as possible,
     the formula used in the real-world question on which these homework
     exercises are loosely based.
  */

  double lat1 = y_0;
  double lat2 = y_1;
  double lon1 = x_0;
  double lon2 = x_1;

  double dLat = deg_to_rad(lat2 - lat1);
  double dLon = deg_to_rad(lon2 - lon1);
  lat1 = deg_to_rad(lat1);
  lat2 = deg_to_rad(lat2);

  double a =
      square(sin(dLat / 2.0)) + cos(lat1) * cos(lat2) * square(sin(dLon / 2));
  double c = 2.0 * asin(sqrt(a));

  double result = radius * c;

  return result;
}
