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

class VertexGraph {
public:
    struct Node {
        RealCoordinate coord;
        std::vector<Node*> connected;
    };
    struct iterator {
        iterator(VertexGraph::Node* ptr);
        iterator(const iterator& other);
        ~iterator();
        bool operator==(const iterator& other);
        bool operator!=(const iterator& other);
        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
        Node& operator*();
        const Node& operator*() const;
        Node* operator->();
    private:
        VertexGraph::Node* data_ptr;
    };
    VertexGraph(VertexGraph::Node* graph, std::vector<VertexGraph::Node*>& refs);
    VertexGraph(const VertexGraph& other);
    VertexGraph(VertexGraph&& other);
    ~VertexGraph();
    VertexGraph& operator=(const VertexGraph& other);
    VertexGraph& operator=(VertexGraph&& other);
    VertexGraph::Node* get_head();
    std::vector<VertexGraph::Node*> get_vertices(); 
    iterator begin();
    iterator end();
private:
    VertexGraph::Node* data = nullptr;
    std::vector<VertexGraph::Node*> refs;
};

class RegionGraph {
public: 
    struct Node {
        RealCoordinate site;
        std::vector<RealCoordinate> vertices;
        std::vector<RegionGraph::Node*> adjacent;
        RealCoordinate centroid();
        double area();
    };
    struct iterator {
        iterator(RegionGraph::Node* ptr);
        iterator(const iterator& other);
        ~iterator();
        bool operator==(const iterator& other);
        bool operator!=(const iterator& other);
        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
        Node& operator*();
        const Node& operator*() const;
        Node* operator->();
    private:
        RegionGraph::Node* data_ptr;
    };
    RegionGraph(RegionGraph::Node* graph, std::vector<RegionGraph::Node*>& refs);
    RegionGraph(const RegionGraph& other);
    RegionGraph(RegionGraph&& other);
    ~RegionGraph();
    RegionGraph& operator=(const RegionGraph& other);
    RegionGraph& operator=(RegionGraph&& other); 
    RegionGraph::Node* get_head();
    std::vector<RegionGraph::Node*> get_regions();
    iterator begin();
    iterator end();
private:
    RegionGraph::Node* data = nullptr;
    std::vector<RegionGraph::Node*> refs;
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
    void relax_iters(const Bbox& bounds, int iters);
    std::vector<RealCoordinate> get_seeds();
    VertexGraph get_vertex_graph();
    RegionGraph get_region_graph();

private:
    int nregions;
    Bbox bounds;
    std::mt19937_64 rng;
    //std::vector<std::vector<int>> diagram;
    std::vector<RealCoordinate> seeds;
    Impl::EventManager event_manager;
    Impl::EventQueue event_queue;
    Impl::BeachLine beach_line;
    //Impl::Dcel dcel;
    std::vector<Impl::Region*> regions;
    std::vector<Impl::HalfEdge*> half_edges;
    std::vector<RealCoordinate*> vertices;  // change to Impl::Vertex?
    Impl::HalfEdge* internal_half_edges = nullptr;
    RealCoordinate* internal_vertices = nullptr;
    Impl::Region* region_data = nullptr;
    int next_half_edge_index = 0;
    int next_vertex_index = 0;
    int next_region_id = 0;
    std::vector<Impl::HalfEdge*> additional_half_edges;
    std::vector<RealCoordinate*> additional_vertices;

    Impl::Region* new_region(const RealCoordinate& c);
    Impl::HalfEdge* new_interior_edge(Impl::Region* region);
    RealCoordinate* new_interior_vertex(const RealCoordinate& c);
    std::vector<RealCoordinate> filter_duplicate_seeds(
        std::vector<RealCoordinate>& seeds
    );
    void initialize();    
    void compute();
    void site_event(RealCoordinate site);
    void intersection_event(const Impl::Event& event);
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

inline VertexGraph::VertexGraph(
    VertexGraph::Node* data, std::vector<VertexGraph::Node*>& refs
): data(data), refs(refs) {}

inline VertexGraph::~VertexGraph() { if (data) { delete[] data; } }
    
inline VertexGraph::iterator VertexGraph::begin() {
    return iterator(data);
}

inline VertexGraph::iterator VertexGraph::end() {
    return iterator(data + refs.size());
}

inline VertexGraph::iterator::iterator(VertexGraph::Node* ptr) {
    data_ptr = ptr;
}

inline VertexGraph::iterator::iterator(const iterator& other) {
    data_ptr = other.data_ptr;
}
        
inline VertexGraph::iterator::~iterator() {}

inline bool VertexGraph::iterator::operator==(const iterator& other) {
    return data_ptr == other.data_ptr;
}

inline bool VertexGraph::iterator::operator!=(const iterator& other) {
    return data_ptr != other.data_ptr;
}

inline VertexGraph::iterator& VertexGraph::iterator::operator++() {
    ++data_ptr;
    return *this;
}

inline VertexGraph::iterator VertexGraph::iterator::operator++(int) {
    iterator old = *this;
    ++data_ptr;
    return old;
}

inline VertexGraph::iterator& VertexGraph::iterator::operator--() {
    --data_ptr;
    return *this;
}

inline VertexGraph::iterator VertexGraph::iterator::operator--(int) {
    iterator old = *this;
    --data_ptr;
    return old;
}
    
inline VertexGraph::Node& VertexGraph::iterator::operator*() {
    return *data_ptr;
}

inline const VertexGraph::Node& VertexGraph::iterator::operator*() const {
    return *data_ptr;
}
    
inline VertexGraph::Node* VertexGraph::iterator::operator->() {
    return data_ptr;
}

inline RegionGraph::RegionGraph(
    RegionGraph::Node* data, std::vector<RegionGraph::Node*>& refs
): data(data), refs(refs) {}

inline RegionGraph::~RegionGraph() { if (data) { delete[] data; } }

inline RegionGraph::iterator::iterator(RegionGraph::Node* ptr) {
    data_ptr = ptr;
}

inline RegionGraph::iterator::iterator(const iterator& other) {
    data_ptr = other.data_ptr;
}
        
inline RegionGraph::iterator::~iterator() {}

inline bool RegionGraph::iterator::operator==(const iterator& other) {
    return data_ptr == other.data_ptr;
}

inline bool RegionGraph::iterator::operator!=(const iterator& other) {
    return data_ptr != other.data_ptr;
}

inline RegionGraph::iterator& RegionGraph::iterator::operator++() {
    ++data_ptr;
    return *this;
}

inline RegionGraph::iterator RegionGraph::iterator::operator++(int) {
    iterator old = *this;
    ++data_ptr;
    return old;
}

inline RegionGraph::iterator& RegionGraph::iterator::operator--() {
    --data_ptr;
    return *this;
}

inline RegionGraph::iterator RegionGraph::iterator::operator--(int) {
    iterator old = *this;
    --data_ptr;
    return old;
}
    
inline RegionGraph::Node& RegionGraph::iterator::operator*() {
    return *data_ptr;
}

inline const RegionGraph::Node& RegionGraph::iterator::operator*() const {
    return *data_ptr;
}
    
inline RegionGraph::Node* RegionGraph::iterator::operator->() {
    return data_ptr;    
}

inline RegionGraph::iterator RegionGraph::begin() {
    return iterator(data);
}

inline RegionGraph::iterator RegionGraph::end() {
    return iterator(data + refs.size());
}

inline Calculator::Calculator(int seed): rng(std::mt19937_64(seed)) {}

inline Calculator::~Calculator() {
    if (internal_half_edges) { delete[] internal_half_edges; }
    if (internal_vertices) { delete[] internal_vertices; }
    if (region_data) { delete[] region_data; }
    for (RealCoordinate* vertices : additional_vertices) {
        delete[] vertices;
    }
    for (Impl::HalfEdge* edges : additional_half_edges) {
        delete[] edges;
    }
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

inline RealCoordinate* Calculator::new_interior_vertex(const RealCoordinate& c) {
    RealCoordinate* vertex = &internal_vertices[next_vertex_index++];
    *vertex = c;
    return vertex;
}
} // namespace VoronoiDiagram
