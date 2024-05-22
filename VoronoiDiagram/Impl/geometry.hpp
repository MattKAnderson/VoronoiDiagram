#pragma once
#include <cmath>
#include <vector>
#include <VoronoiDiagram/Coordinate.hpp>

namespace VoronoiDiagram::Impl {    
double euclidean_distance(const RealCoordinate& c1, const RealCoordinate& c2);

RealCoordinate triangle_circumcenter(
    const RealCoordinate& a, const RealCoordinate& b, const RealCoordinate& c
);

std::vector<double> quadratic_roots(double a, double b, double c);

double parabola_x_from_y(double directrix, const RealCoordinate& focus, double y);

RealCoordinate parabola_intercept(
    double directrix, const RealCoordinate& focus1, const RealCoordinate& focus2
);

double parabolae_y_intercept(
    double directrix, const RealCoordinate& focus1, const RealCoordinate& focus2
);
} // namespace VoronoiDiagram::Impl