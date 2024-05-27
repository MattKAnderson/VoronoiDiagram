#pragma once
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Impl/geometry.hpp>
#include <VoronoiDiagram/Impl/Dcel.hpp>

namespace VoronoiDiagram::Impl {
struct Arc {
    Arc() {}
    Arc(const RealCoordinate& focus, Region* region): focus(focus), region(region) {};
    RealCoordinate focus;
    Region* region = nullptr;
    bool red = true;
    int event_id = -1;
    Arc* left = nullptr;
    Arc* right = nullptr;
    Arc* parent = nullptr;
    Arc* lower = nullptr;
    Arc* upper = nullptr;
    HalfEdge* upper_edge = nullptr;
    HalfEdge* lower_edge = nullptr;
};

class BeachLine {
public:
    BeachLine();
    BeachLine(const BeachLine& other) = delete;
    BeachLine& operator=(const BeachLine& other) = delete;
    ~BeachLine();
    Arc* new_arc(const RealCoordinate& r1, Region* region);
    Arc* find_intersected_arc(const RealCoordinate& c);
    void set_head(Arc* arc);
    Arc* get_head();
    Arc* get_lowest();
    void insert_arc_above(Arc* arc, Arc* new_arc);
    void insert_arc_below(Arc* arc, Arc* new_arc);
    void remove_arc(Arc* arc);
    void reserve(int n);
    void reset();

private:
    const static int POOL_ALLOC_SIZE = 512; 
    Arc* head = nullptr;
    std::vector<Arc*> arc_pools;
    int pool_index = 0;
    int next_arc_index = 0;
    std::vector<Arc*> available_arcs;
    //std::vector<Arc*> closed_regions;
    void insert_balance(Arc* arc);
    void delete_balance(Arc* arc);
    Arc* insert_local_balance(Arc* arc);
    Arc* rotate_left(Arc* arc);
    Arc* rotate_right(Arc* arc);
    void flip_colors(Arc* arc);
    bool is_red(Arc* arc);
};

inline BeachLine::BeachLine() {
    arc_pools.push_back(new Arc[POOL_ALLOC_SIZE]);
}

inline BeachLine::~BeachLine() {
    for (Arc* pool : arc_pools) {
        delete[] pool;
    }
}

inline void BeachLine::set_head(Arc* arc) {
    head = arc;
}

inline Arc* BeachLine::new_arc(const RealCoordinate& focus, Region* region) {
    Arc* arc = nullptr;
    if (!available_arcs.empty()) {
        arc = available_arcs.back(); available_arcs.pop_back();
        *arc = Arc(focus, region);
    }
    else {
        if (next_arc_index < POOL_ALLOC_SIZE) { 
            arc = &arc_pools[pool_index][next_arc_index++];
        }
        else {
            if (pool_index == arc_pools.size() - 1) {
                arc_pools.push_back(new Arc[POOL_ALLOC_SIZE]);
            }
            ++pool_index;                
            next_arc_index = 1;
            arc = &arc_pools[pool_index][0];
        }
        *arc = Arc(focus, region);
    }
    return arc;
}

inline Arc* BeachLine::get_head() {
    return head;
}

inline void BeachLine::reset() {
    head = nullptr;
    pool_index = 0;
    next_arc_index = 0;
}
} // namespace VoronoiDiagram::Impl