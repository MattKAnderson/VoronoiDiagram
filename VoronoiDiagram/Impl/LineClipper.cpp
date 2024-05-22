#include <VoronoiDiagram/Impl/LineClipper.hpp>

namespace VoronoiDiagram::Impl {
int LineClipper::outcode(const RealCoordinate& c) {
    int code = INSIDE;
    if (c.x < bounds.xmin) {
        code |= LEFT;
    }
    else if (c.x > bounds.xmax) {
        code |= RIGHT;
    }
    if (c.y < bounds.ymin) {
        code |= BOTTOM;
    }
    else if (c.y > bounds.ymax) {
        code |= TOP;
    }
    return code;
}

bool LineClipper::CohenSutherlandClip(
    const RealCoordinate& a, const RealCoordinate& b
) {
    a_ = a;
    b_ = b;
    int code_a = outcode(a_);
    int code_b = outcode(b_);
    while(true) {
        if (!(code_a | code_b)) { return true; }
        else if (code_a & code_b) { return false; }
        
        double x, y;
        int code_out = code_a > code_b ? code_a : code_b;
        if (code_out & TOP) {
            x = a_.x + (bounds.ymax - a_.y) * (b_.x - a_.x) / (b_.y - a_.y);
            y = bounds.ymax;
        }
        else if (code_out & BOTTOM) {
            x = a_.x + (bounds.ymin - a_.y) * (b_.x - a_.x) / (b_.y - a_.y);
            y = bounds.ymin;
        }
        else if (code_out & RIGHT) {
            y = a_.y + (bounds.xmax - a_.x) * (b_.y - a_.y) / (b_.x - a_.x);
            x = bounds.xmax;
        }
        else {
            y = a_.y + (bounds.xmin - a_.x) * (b_.y - a_.y) / (b_.x - a_.x);
            x = bounds.xmin;
        }

        if (code_out == code_a) {
            a_ = {x, y};
            code_a = outcode(a_);
        }
        else {
            b_ = {x, y};
            code_b = outcode(b_);
        }
    }
}
} // namespace VoronoiDiagram::Impl