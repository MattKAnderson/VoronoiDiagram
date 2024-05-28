#pragma once
#include <list>
#include <limits>
#include <vector>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <VoronoiDiagram/Bbox.hpp>
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/Impl/BeachLine.hpp>
#include <VoronoiDiagram/Impl/Dcel.hpp>
#include <VoronoiDiagram/Impl/Event.hpp>
#include <VoronoiDiagram/Impl/geometry.hpp>
#include <VoronoiDiagram/Impl/LineClipper.hpp>

namespace VoronoiDiagram {
struct VertexNode {
    RealCoordinate coord;
    std::vector<VertexNode*> connected;
    VertexNode();
    VertexNode(const RealCoordinate& coord); // not sure I should have this method
    bool operator==(const VertexNode& other) const; // not sure about this either
};

class VertexGraph {
public:
    VertexGraph(VertexNode* graph, std::vector<VertexNode*>& refs);
    ~VertexGraph();
    VertexNode* get_head();
    std::vector<VertexNode*> get_vertices(); 
private:
    VertexNode* data;
    std::vector<VertexNode*> refs;
};

struct RegionNode {
    RealCoordinate site; 
    std::vector<RealCoordinate> vertices;
    std::vector<RegionNode*> adjacent;
    RealCoordinate centroid();
    double area();
};

class RegionGraph {
public: 
    RegionGraph(RegionNode* graph, std::vector<RegionNode*>& refs);
    ~RegionGraph();
    RegionGraph& operator=(RegionGraph&& other) { 
        if (this == &other) { return *this; } 
        data = other.data; 
        refs = other.refs; 
        other.data = nullptr; 
        other.refs = {}; 
        return *this;
    }
    RegionNode* get_head();
    std::vector<RegionNode*> get_regions();
private:
    RegionNode* data;
    std::vector<RegionNode*> refs;
};

class Calculator {
public:
    Calculator(int seed = 0);
    ~Calculator();
    void generate(int nseeds);
    void generate(int nseeds, const Bbox& box);
    void generate(std::vector<RealCoordinate>& seeds);
    void generate(std::vector<RealCoordinate>& seeds, const Bbox& box);
    void relax(const Bbox& bounds);
    std::vector<RealCoordinate> get_seeds();
    VertexGraph get_vertex_graph();
    RegionGraph get_region_graph();

private:
    int nregions;
    std::mt19937_64 rng;
    //std::vector<std::vector<int>> diagram;
    std::vector<RealCoordinate> seeds;
    Impl::EventPool* event_pool = nullptr;
    Impl::EventQueue event_queue;
    Impl::BeachLine beach_line;
    //Impl::Dcel dcel;
    std::vector<Impl::Region*> regions;
    std::vector<Impl::HalfEdge*> half_edges;
    std::vector<VertexNode*> vertices;  // change to Impl::Vertex?
    Impl::HalfEdge* internal_half_edges = nullptr;
    VertexNode* internal_vertices = nullptr;
    Impl::Region* region_data = nullptr;
    int next_half_edge_index = 0;
    int next_vertex_index = 0;
    int next_region_id = 0;
    std::vector<Impl::HalfEdge*> additional_half_edges;
    std::vector<VertexNode*> additional_vertices;

    Impl::Region* new_region(const RealCoordinate& c);
    Impl::HalfEdge* new_interior_edge(Impl::Region* region);
    VertexNode* new_interior_vertex(const RealCoordinate& c);
    void initialize();
    void compute(std::vector<RealCoordinate>& seeds);
    void site_event(RealCoordinate site);
    void intersection_event(Impl::Event* event);
    void bound();
    void crop(const Bbox& box);
    void connect_DCEL_exterior(
        std::list<std::pair<RealCoordinate, Impl::HalfEdge*>>& exterior, 
        const Bbox& box
    );
    RealCoordinate clip_infinite_edge(Impl::HalfEdge*, const Bbox& box);
    std::vector<RealCoordinate> generate_seeds(int nseeds, const Bbox& box);
    std::vector<RealCoordinate> region_centroids();
};

inline VertexNode::VertexNode() { 
    //connected.reserve(3); 
}

inline VertexNode::VertexNode(const RealCoordinate& coord): coord(coord) { 
    //connected.reserve(3); 
}

inline bool VertexNode::operator==(const VertexNode& other) const { 
    return coord == other.coord; 
}

inline VertexGraph::VertexGraph(
    VertexNode* data, std::vector<VertexNode*>& refs
): data(data), refs(refs) {}

inline VertexGraph::~VertexGraph() { if (data) { delete[] data; } }

inline RegionGraph::RegionGraph(
    RegionNode* data, std::vector<RegionNode*>& refs
): data(data), refs(refs) {}

inline RegionGraph::~RegionGraph() { if (data) { delete[] data; } }

inline Calculator::Calculator(int seed): rng(std::mt19937_64(seed)) {}

inline Calculator::~Calculator() {
    if (internal_half_edges) { delete[] internal_half_edges; }
    if (internal_vertices) { delete[] internal_vertices; }
    if (region_data) { delete[] region_data; }
    for (VertexNode* vertices : additional_vertices) {
        delete[] vertices;
    }
    for (Impl::HalfEdge* edges : additional_half_edges) {
        delete[] edges;
    }
    delete event_pool;
}

inline Impl::Region* Calculator::new_region(const RealCoordinate& site) {
    Impl::Region* region = &region_data[next_region_id++];
    region->seed = site;
    regions.push_back(region);
    return region;
}

inline Impl::HalfEdge* Calculator::new_interior_edge(Impl::Region* region) {
    Impl::HalfEdge* edge = &internal_half_edges[next_half_edge_index++];
    edge->region = region;
    return edge;
}

inline VertexNode* Calculator::new_interior_vertex(const RealCoordinate& c) {
    VertexNode* vertex = &internal_vertices[next_vertex_index++];
    vertex->coord = c;
    return vertex;
}
} // namespace VoronoiDiagram

//namespace std {
//template <>
//struct hash<VoronoiDiagram::VertexNode> {
//    size_t operator()(const VoronoiDiagram::VertexNode& n) const {
//        return hash<VoronoiDiagram::RealCoordinate>()(n.coord);
//    }
//};   
//} // namespace std