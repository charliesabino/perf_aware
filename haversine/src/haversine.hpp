#pragma once

inline double EARTH_RADIUS = 6372.8;

// NOTE(casey): EarthRadius is generally expected to be 6372.8
double haversine(double x_0, double y_0, double x_1, double y_1, double radius);
