#include <cmath>

using f64 = double;

static f64 square(f64 a) { return a * a; }

static f64 deg_to_rad(f64 Degrees) { return 0.01745329251994329577 * Degrees; }

// NOTE(casey): EarthRadius is generally expected to be 6372.8
static f64 haversine(f64 x_0, f64 y_0, f64 x_1, f64 y_1, f64 radius) {
  /* NOTE(casey): This is not meant to be a "good" way to calculate the
     Haversine distance. Instead, it attempts to follow, as closely as possible,
     the formula used in the real-world question on which these homework
     exercises are loosely based.
  */

  f64 lat1 = y_0;
  f64 lat2 = y_1;
  f64 lon1 = x_0;
  f64 lon2 = x_1;

  f64 dLat = deg_to_rad(lat2 - lat1);
  f64 dLon = deg_to_rad(lon2 - lon1);
  lat1 = deg_to_rad(lat1);
  lat2 = deg_to_rad(lat2);

  f64 a =
      square(sin(dLat / 2.0)) + cos(lat1) * cos(lat2) * square(sin(dLon / 2));
  f64 c = 2.0 * asin(sqrt(a));

  f64 result = radius * c;

  return result;
}
