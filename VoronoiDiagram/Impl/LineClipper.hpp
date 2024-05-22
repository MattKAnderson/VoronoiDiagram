#pragma once
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Bbox.hpp>

namespace VoronoiDiagram::Impl {
class LineClipper {
public:
    LineClipper(const Bbox& bounds): bounds(bounds) {}
    bool CohenSutherlandClip(const RealCoordinate& a, const RealCoordinate& b);
    RealCoordinate get_clipped_a() { return a_; };
    RealCoordinate get_clipped_b() { return b_; };
private:
    Bbox bounds;
    RealCoordinate a_;
    RealCoordinate b_;
    int outcode(const RealCoordinate& c);
    const static int INSIDE = 0;
    const static int LEFT = 1;
    const static int RIGHT = 2;
    const static int BOTTOM = 4;
    const static int TOP = 8;
};
} // namespace VoronoiDiagram::Impl