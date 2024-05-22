#pragma once
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Impl/geometry.hpp>

namespace VoronoiDiagram::Impl {
    struct HalfEdge;
    struct Region {
        RealCoordinate seed;
        HalfEdge* an_edge = nullptr;
    };
    struct HalfEdge {
        Region* region = nullptr;
        HalfEdge* prev = nullptr;
        HalfEdge* next = nullptr;
        HalfEdge* twin = nullptr;
        int origin_id = -1;
    };
    class Dcel {
        
    };
}