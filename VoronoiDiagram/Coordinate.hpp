#pragma once
#include <vector>

namespace VoronoiDiagram {
struct RealCoordinate {
    double x, y;
    RealCoordinate() {}
    RealCoordinate(double x, double y): x(x), y(y) {}
    bool operator==(const RealCoordinate& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const RealCoordinate& other) const {
        return x != other.x && y != other.y;
    }
};
} // namespace VoronoiDiagram

namespace std {
template <> 
struct hash<VoronoiDiagram::RealCoordinate> {
    size_t operator()(const VoronoiDiagram::RealCoordinate& c) const {
        size_t hx = hash<double>()(c.x);
        size_t hy = hash<double>()(c.y);
        return hx ^ (hy << 1);
    }
};
} // namespace std