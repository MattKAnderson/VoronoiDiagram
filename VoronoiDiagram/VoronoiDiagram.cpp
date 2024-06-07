#include <VoronoiDiagram/VoronoiDiagram.hpp>

namespace VoronoiDiagram {

VertexGraph::VertexGraph(const VertexGraph& other) {
    refs.reserve(other.refs.size());
    data = new VertexGraph::Node[refs.size()];
    for (int i = 0; i < other.refs.size(); i++) {
        size_t pos = std::distance(other.data, other.refs[i]);
        data[pos].coord = other.refs[i]->coord;
        data[pos].connected.reserve(other.refs[i]->connected.size());
        for (VertexGraph::Node* adj : other.refs[i]->connected) {
            size_t adj_pos = std::distance(other.data, adj);
            data[pos].connected.push_back(data + adj_pos);
        }
    }
}

VertexGraph::VertexGraph(VertexGraph&& other) {
    std::swap(refs, other.refs);
    std::swap(data, other.data);
}

VertexGraph& VertexGraph::operator=(const VertexGraph& other) {
    if (this == &other) {
        return *this;
    }
    VertexGraph tmp(other);
    std::swap(refs, tmp.refs);
    std::swap(data, tmp.data);
    return *this;
}

VertexGraph& VertexGraph::operator=(VertexGraph&& other) {
    std::swap(refs, other.refs);
    std::swap(data, other.data);
    return *this;
}

VertexGraph::Node* VertexGraph::get_head() { 
    if (refs.size() == 0) { return nullptr; }
    else { return refs[0]; }
}

std::vector<VertexGraph::Node*> VertexGraph::get_vertices() {
    return refs;
}

RegionGraph::RegionGraph(const RegionGraph& other) {
    refs.reserve(other.refs.size());
    data = new RegionGraph::Node[refs.size()];
    for (int i = 0; i < other.refs.size(); i++) {
        size_t pos = std::distance(other.data, other.refs[i]);
        data[pos].site = other.refs[i]->site;
        data[pos].vertices = other.refs[i]->vertices;
        data[pos].adjacent.reserve(other.refs[i]->adjacent.size());
        for (RegionGraph::Node* adj : other.refs[i]->adjacent) {
            size_t adj_pos = std::distance(other.data, adj);
            data[pos].adjacent.push_back(data + adj_pos);
        }
    }
}

RegionGraph::RegionGraph(RegionGraph&& other) {
    std::swap(data, other.data);
    std::swap(refs, other.refs);
}

RegionGraph& RegionGraph::operator=(const RegionGraph& other) {
    if (this == &other) { return *this; }
    RegionGraph tmp(other);
    std::swap(data, tmp.data);
    std::swap(refs, tmp.refs);
    return *this;
}

RegionGraph& RegionGraph::operator=(RegionGraph&& other) {
    std::swap(data, other.data);
    std::swap(refs, other.refs);
    return *this;
}

RegionGraph::Node* RegionGraph::get_head() {
    if (refs.size() == 0) { return nullptr; }
    else { return refs[0]; }
}

std::vector<RegionGraph::Node*> RegionGraph::get_regions() {
    return refs;
}

RealCoordinate RegionGraph::Node::centroid() {
    return Impl::polygon_centroid(vertices);
}

double RegionGraph::Node::area() {
    double a = 0.0;
    RealCoordinate last_v = vertices.back();
    for (int i = 0; i < vertices.size(); i++) {
        const RealCoordinate& v = vertices[i];
        a += (v.x * last_v.y) - (last_v.x * v.y);
        last_v = v;
    }
    a /= 2;
    return a;
}

void Calculator::generate(int nseeds) {
    seeds = generate_seeds(nseeds, {0.0, 1.0, 0.0, 1.0});
    generate(seeds);
}

void Calculator::generate(int nseeds, const Bbox& box) {
    seeds = generate_seeds(nseeds, box);
    generate(seeds, box);
}

void Calculator::generate(
    std::vector<RealCoordinate>& seeds
) {
    this->seeds = filter_duplicate_seeds(seeds);
    bounds = {
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min()
    };
    for (auto& seed : seeds) {
        bounds.xmin = std::min(bounds.xmin, seed.x);
        bounds.xmax = std::max(bounds.xmax, seed.x);
        bounds.ymin = std::min(bounds.ymin, seed.y);
        bounds.ymax = std::min(bounds.ymax, seed.y);
    }
    // TODO determin bounding box;
    compute();
    bound();
}

void Calculator::generate(
    std::vector<RealCoordinate>& seeds, const Bbox& box
) {
    this->seeds = filter_duplicate_seeds(seeds);
    this->bounds = box;
    compute();
    crop(box);
}

void Calculator::relax(const Bbox& bounds) {
    this->seeds = region_centroids();
    this->bounds = bounds;
    // does full re-initialization for now. Obvious optimization is only 
    // to de-allocate and re-allocate memory that needs to be
    compute();
    crop(bounds);
}

void Calculator::relax_iters(const Bbox& bounds, int iters) {
    for (int i = 0; i < iters; ++i) {
        relax(bounds);
    }
}


std::vector<RealCoordinate> Calculator::get_seeds() {
    return seeds;
}

VertexGraph Calculator::get_vertex_graph() {
    std::vector<VertexGraph::Node*> vertex_refs; 
    VertexGraph::Node* new_vertices = new VertexGraph::Node[vertices.size()];
    vertex_refs.reserve(vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        vertex_refs.push_back(&new_vertices[i]);
        new_vertices[i].coord = *vertices[i];
        new_vertices[i].connected.reserve(4);
    }
    for (Impl::HalfEdge* half_edge : half_edges) {
        if (half_edge->twin->origin_id == -1) {
            std::cout << "Caught -1 id" << std::endl;
        }
        else if (half_edge->twin->origin_id >= vertices.size()) {
            std::cout << "Caught out of bounds id" << std::endl;
            RealCoordinate c = *vertices[half_edge->origin_id];
            std::cout << "this coord: " << c.x << ", " << c.y << std::endl;
        }
        else {
            new_vertices[half_edge->origin_id].connected.push_back(
                &new_vertices[half_edge->twin->origin_id]
            );
        }
    }
    return VertexGraph(new_vertices, vertex_refs);
}

RegionGraph Calculator::get_region_graph() {
    std::vector<RegionGraph::Node*> nodes; nodes.reserve(nregions);
    RegionGraph::Node* nodes_array = new RegionGraph::Node[nregions];
    int next_id = 0;
    std::unordered_map<Impl::Region*, RegionGraph::Node*> node_map;
    for (int i = 0; i < nregions; i++) {
        nodes_array[i].adjacent.reserve(8);
        nodes_array[i].vertices.reserve(8);
        nodes_array[i].site = regions[i]->seed;
        nodes.push_back(&nodes_array[i]);
        node_map.insert({regions[i], nodes.back()});
    }
    for (int i = 0; i < nregions; i++) {
        RegionGraph::Node* this_region = node_map[regions[i]];
        Impl::HalfEdge* edge_ptr = regions[i]->an_edge;
        while (edge_ptr->next != regions[i]->an_edge) {
            this_region->vertices.push_back(*vertices[edge_ptr->origin_id]);
            if (edge_ptr->twin->region != nullptr) {
                this_region->adjacent.push_back(node_map[edge_ptr->twin->region]);
            }
            edge_ptr = edge_ptr->next;
        }
        if (edge_ptr->twin->region != nullptr) {
            this_region->adjacent.push_back(node_map[edge_ptr->twin->region]);
        }
        this_region->vertices.push_back(*vertices[edge_ptr->origin_id]);
    }
    return RegionGraph(nodes_array, nodes);
}

std::vector<RealCoordinate> Calculator::filter_duplicate_seeds(
    std::vector<RealCoordinate>& seeds
) {
    std::vector<RealCoordinate> filtered; filtered.reserve(seeds.size());
    std::unordered_set<RealCoordinate> seen; seen.reserve(seeds.size());
    for (const RealCoordinate& seed : seeds) {
        if (seen.find(seed) == seen.end()) { 
            filtered.push_back(seed);
            seen.insert(seed);
        }
    }
    return std::move(filtered);
}

void Calculator::initialize() {
    if (region_data != nullptr) {
        delete[] region_data;
    }
    region_data = new Impl::Region[nregions];
    regions = {};
    half_edges = {};
    half_edges.reserve(2 * (nregions * 3 - 6));
    if (internal_half_edges != nullptr) {
        delete[] internal_half_edges;
    }
    internal_half_edges = new Impl::HalfEdge[6 * (nregions) - 12];
    if (internal_vertices != nullptr) {
        delete[] internal_vertices;
    }
    internal_vertices = new RealCoordinate[2 * nregions - 5];
    vertices = {};
    vertices.reserve(3 * nregions - 6);
    beach_line.reset();
    event_manager = Impl::EventManager();
    event_queue = Impl::EventQueue();
    event_queue.initialize(&event_manager, nregions, bounds.xmin, bounds.xmax);
    next_half_edge_index = 0;
    next_vertex_index = 0;
    next_region_id = 0;
}
    
void Calculator::compute() {
    nregions = seeds.size();
    initialize();

    for (const RealCoordinate& seed : seeds) { 
        event_queue.insert(event_manager.create(seed));
    }
    int event_id = event_queue.consume_next();
    const RealCoordinate& s1 = event_manager.get(event_id).point;
    beach_line.set_head(beach_line.new_arc(s1, new_region(s1)));
    
    while (!event_queue.empty()) {
        event_id = event_queue.consume_next();
        const Impl::Event& event = event_manager.get(event_id);
        //std::cout << "Sweepline: " << event.sweepline << std::endl;
        if (event.associated_arc == nullptr) { 
            site_event(event.point);
        }
        else if (event.associated_arc) {
            intersection_event(event);
        }
        event_manager.remove(event_id);
    }
}
    
void Calculator::site_event(RealCoordinate site) {
    using namespace Impl;
    Arc* arc = beach_line.find_intersected_arc(site);
    Arc* new_arc = beach_line.new_arc(site, new_region(site)); 
    Arc* split_arc = beach_line.new_arc(arc->focus, arc->region); 

    beach_line.insert_arc_above(arc, new_arc);
    beach_line.insert_arc_above(new_arc, split_arc);

    new_arc->upper_edge = new_interior_edge(new_arc->region); 
    half_edges.push_back(new_arc->upper_edge);
    new_arc->lower_edge = new_arc->upper_edge;
    new_arc->region->an_edge = new_arc->upper_edge;
    split_arc->upper_edge = arc->upper_edge;
    arc->upper_edge = new_interior_edge(arc->region); 
    half_edges.push_back(arc->upper_edge);
    split_arc->lower_edge = arc->upper_edge;
    arc->upper_edge->twin = new_arc->upper_edge;
    new_arc->lower_edge->twin = arc->upper_edge;
    arc->region->an_edge = arc->upper_edge;
    new_arc->region->an_edge = new_arc->upper_edge;

    Arc* upper_upper = split_arc->upper;
    Arc* lower_lower = arc->lower;
    if (upper_upper) {
        RealCoordinate intersect = triangle_circumcenter(
            site, split_arc->focus, upper_upper->focus
        );
        if (site.y < intersect.y) {
            double dist = euclidean_distance(split_arc->focus, intersect); 
            if (split_arc->event_id != -1) {
                event_queue.remove(split_arc->event_id);
                event_manager.remove(split_arc->event_id);
            }
            int event_id = event_manager.create(intersect.x + dist, intersect, split_arc);
            split_arc->event_id = event_id;
            event_queue.insert(event_id);
        }
    } 
    if (lower_lower) {
        RealCoordinate intersect = triangle_circumcenter(
            site, arc->focus, lower_lower->focus
        );
        if (site.y > intersect.y) {
            double dist = euclidean_distance(arc->focus, intersect);
            if (arc->event_id != -1) {
                event_queue.remove(arc->event_id);
                event_manager.remove(arc->event_id);
            }
            int event_id = event_manager.create(intersect.x + dist, intersect, arc);
            arc->event_id = event_id;
            event_queue.insert(event_id);
        }
    }
}
    
void Calculator::intersection_event(const Impl::Event& event) {
    using namespace Impl;
    RealCoordinate intersect = event.point;
    double event_x = event.sweepline;
    Arc* arc = event.associated_arc;
    Arc* u_arc = arc->upper;
    Arc* l_arc = arc->lower;
    vertices.push_back(new_interior_vertex(intersect));

    arc->upper_edge->origin_id = vertices.size() - 1;
    arc->upper_edge->prev = arc->lower_edge;
    arc->lower_edge->next = arc->upper_edge;
    HalfEdge* new_upper_half_edge = new_interior_edge(u_arc->region); 
    HalfEdge* new_lower_half_edge = new_interior_edge(l_arc->region); 
    half_edges.push_back(new_upper_half_edge);
    half_edges.push_back(new_lower_half_edge);
    new_upper_half_edge->twin = new_lower_half_edge;
    new_upper_half_edge->origin_id = vertices.size() - 1;
    new_upper_half_edge->prev = u_arc->lower_edge;
    u_arc->lower_edge->next = new_upper_half_edge;
    u_arc->lower_edge = new_upper_half_edge;
    new_lower_half_edge->twin = new_upper_half_edge;
    new_lower_half_edge->next = l_arc->upper_edge;
    l_arc->upper_edge->prev = new_lower_half_edge;
    l_arc->upper_edge->origin_id = vertices.size() - 1;
    l_arc->upper_edge = new_lower_half_edge;

    beach_line.remove_arc(arc);

    const RealCoordinate& fu = u_arc->focus;
    const RealCoordinate& fl = l_arc->focus;
    Arc* uu_arc = u_arc->upper;
    Arc* ll_arc = l_arc->lower;
    if (uu_arc && fl != uu_arc->focus && arc->focus != uu_arc->focus) {
        const RealCoordinate& fuu = uu_arc->focus;
        RealCoordinate new_intersect = triangle_circumcenter(fl, fu, fuu);
        if ((fu.y - fl.y) * (new_intersect.x - intersect.x) >= 0.0) {
            double known_at_x = new_intersect.x + euclidean_distance(new_intersect, fu);
            RealCoordinate u_edge = parabola_intercept(event_x, fuu, fu);
            if ((fuu.y - fu.y) * (new_intersect.x - u_edge.x) >= 0.0) {
                int event_id = event_manager.create(known_at_x, new_intersect, u_arc);
                if (u_arc->event_id != -1) {
                    event_queue.remove(u_arc->event_id);
                    event_manager.remove(u_arc->event_id);
                }
                u_arc->event_id = event_id;
                event_queue.insert(event_id);
            }
        }

    }
    if (ll_arc && u_arc->focus != ll_arc->focus && arc->focus != ll_arc->focus) {
        const RealCoordinate& fll = ll_arc->focus;
        RealCoordinate new_intersect = triangle_circumcenter(fu, fl, fll);
        if ((fu.y - fl.y) * (new_intersect.x  - intersect.x) >= 0.0) {
            double known_at_x = new_intersect.x + euclidean_distance(new_intersect, fl);
            RealCoordinate l_edge = parabola_intercept(event_x, fl, fll);
            if ((fl.y - fll.y) * (new_intersect.x - l_edge.x) >= 0.0) {
                int event_id = event_manager.create(known_at_x, new_intersect, l_arc);
                if (l_arc->event_id != -1) {
                    event_queue.remove(l_arc->event_id);
                    event_manager.remove(l_arc->event_id);
                }
                l_arc->event_id = event_id;
                event_queue.insert(event_id);               
            }
        }
    }
}
    
void Calculator::bound() {
    using namespace Impl;
    double xmin = std::numeric_limits<double>::max();
    double ymin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::min();
    double ymax = std::numeric_limits<double>::min();
    for (RealCoordinate* v : vertices) {
        xmin = std::min(xmin, v->x);
        ymin = std::min(ymin, v->y);
        xmax = std::max(xmax, v->x);
        ymax = std::max(ymax, v->y);
    }
    Bbox bounds = {xmin, xmax, ymin, ymax};

    std::list<std::pair<RealCoordinate, HalfEdge*>> exterior;
    Arc* lower = beach_line.get_lowest();
    Arc* upper = lower->upper;
    while (upper != nullptr) {
        HalfEdge* edge = upper->lower_edge->origin_id != -1 ? lower->upper_edge : upper->lower_edge;
        RealCoordinate v = clip_infinite_edge(edge, bounds);
        exterior.emplace_back(v, edge); 
        lower = upper;
        upper = upper->upper;
    } 
    connect_DCEL_exterior(exterior, bounds);
}
    
bool outside_bbox(const RealCoordinate& c, const Bbox& bounds) {
    return c.x < bounds.xmin || c.x > bounds.xmax || c.y < bounds.ymin 
           || c.y > bounds.ymax;
}

bool inside_bbox(const RealCoordinate& c, const Bbox& bounds) {
    return c.x >= bounds.xmin && c.x <= bounds.xmax && c.y >= bounds.ymin 
           && c.y <= bounds.ymax;
}

bool interior_of_bbox(const RealCoordinate& c, const Bbox& bounds) {
    return c.x > bounds.xmin && c.x < bounds.xmax && c.y > bounds.ymin 
           && c.y < bounds.ymax;
}

bool on_bbox_bounds(const RealCoordinate& c, const Bbox& bounds) {
    return c.x == bounds.xmin || c.x == bounds.xmax || c.y == bounds.ymin 
           || c.y == bounds.ymax;
}

bool is_before_on_bbox_exterior(
    const RealCoordinate& a, const RealCoordinate& b, const Bbox& bounds
) { 
    if (a.x == bounds.xmin) {
        if (b.x == bounds.xmin) { return a.y > b.y; }
        else { return a.y != bounds.ymax; }
    }
    if (a.y == bounds.ymin) {
        if (b.y == bounds.ymin) { return a.x < b.x; }
        else if (b.x == bounds.xmin) { return false; }
        else { return true; }
    }
    if (a.x == bounds.xmax) {
        if (b.x == bounds.xmax) { return a.y < b.y; }
        else if (b.y == bounds.ymax) { return true; }
        else { return false; }
    }
    return b.y == bounds.ymax && a.x > b.x;
}


class crop_compare {
public:
    crop_compare(const Bbox& bounds): bounds(bounds) {}
    bool operator()(
        const std::pair<RealCoordinate, Impl::HalfEdge*>& a,
        const std::pair<RealCoordinate, Impl::HalfEdge*>& b
    ) {
        return is_before_on_bbox_exterior(a.first, b.first, bounds);
    }
private:
    Bbox bounds;
};

void Calculator::crop(const Bbox& bounds) {
    using namespace Impl;
    LineClipper clipper(bounds);
    crop_compare comp(bounds);
    std::list<std::pair<RealCoordinate, HalfEdge*>> exterior;
    for (HalfEdge* edge : half_edges) {
        if (edge->origin_id == -1) {
            const RealCoordinate& twin_c = *vertices[edge->twin->origin_id];
            if (interior_of_bbox(twin_c, bounds)) { 
                RealCoordinate v = clip_infinite_edge(edge, bounds);
                exterior.emplace_back(v, edge);
            }
            continue;
        }
        const RealCoordinate& c = *vertices[edge->origin_id];
        if (outside_bbox(c, bounds)) {
            if (edge->twin->origin_id == -1) { continue; }
            const RealCoordinate& twin_c = *vertices[edge->twin->origin_id];
            if (!clipper.CohenSutherlandClip(c, twin_c)) { continue; } 
            exterior.emplace_back(clipper.get_clipped_a(), edge);
        }
        else if (on_bbox_bounds(c, bounds)) {
            exterior.emplace_back(c, edge);
        }
    }
    exterior.sort(comp);

    connect_DCEL_exterior(exterior, bounds);

    std::vector<HalfEdge*> cropped_half_edges;
    std::vector<RealCoordinate*> cropped_vertices;
    std::vector<Region*> cropped_regions; 
    std::vector<int> vertex_id_map(vertices.size(), -1);
    for (int i = 0; i < vertices.size(); i++) {
        if (inside_bbox(*vertices[i], bounds)) {
            vertex_id_map[i] = cropped_vertices.size();
            cropped_vertices.push_back(vertices[i]);
        }
    }
    cropped_half_edges.reserve(6 * cropped_vertices.size());
    for (HalfEdge* edge : half_edges) {
        if (
            edge->origin_id != -1
            && inside_bbox(*vertices[edge->origin_id], bounds)
            && inside_bbox(*vertices[edge->twin->origin_id], bounds)
        ) { 
            cropped_half_edges.push_back(edge);
            if (edge->region) {
                edge->region->an_edge = edge;
            }
        }
    }
    for (HalfEdge* edge : cropped_half_edges) {
        edge->origin_id = vertex_id_map[edge->origin_id];
    }
    vertices = std::move(cropped_vertices);
    half_edges = std::move(cropped_half_edges);
    for (Region* region : regions) {
        HalfEdge* edge = region->an_edge->next;
        while (edge != region->an_edge) {
            const RealCoordinate& v = *vertices[edge->origin_id];
            if (interior_of_bbox(v, bounds)) {
                cropped_regions.push_back(region);
                break;             
            }
            else if (on_bbox_bounds(v, bounds)) {
                const RealCoordinate& twin_v = *vertices[edge->twin->origin_id];
                RealCoordinate midpoint = {
                    0.5 * (v.x + twin_v.x), 0.5 * (v.y + twin_v.y)
                };
                if (interior_of_bbox(midpoint, bounds)) {
                    cropped_regions.push_back(region);
                    break;
                }
            }
            edge = edge->next;
        }
    }
    regions = std::move(cropped_regions);
}
    
void Calculator::connect_DCEL_exterior(
    std::list<std::pair<RealCoordinate, Impl::HalfEdge*>>& exterior, 
    const Bbox& bounds
) {
    using namespace Impl;
    RealCoordinate* exterior_vertices = new RealCoordinate[exterior.size() + 4];
    HalfEdge* boundary_half_edges = new HalfEdge[exterior.size() * 2 + 8];
    additional_vertices.push_back(exterior_vertices);
    additional_half_edges.push_back(boundary_half_edges);

    int next_vertex_id = 0;
    int next_half_edge_id = 0;

    RealCoordinate corners[4] = {
        {bounds.xmin, bounds.ymin}, 
        {bounds.xmax, bounds.ymin}, 
        {bounds.xmax, bounds.ymax}, 
        {bounds.xmin, bounds.ymax}
    };
    int corner_to_place = 0;
    for (auto it = exterior.begin(); it != exterior.end(); ++it) {
        for (int i = corner_to_place; i < 4; i++) {
            if (corners[i] == (*it).first) {
                ++corner_to_place;
                break;
            }
            else if (is_before_on_bbox_exterior(corners[i], (*it).first, bounds)) {
                exterior.insert(it, {corners[i], nullptr});
                ++corner_to_place;
            }
            else {
                break;
            }
        }
    }
    for (int i = corner_to_place; i < 4; i++) {
        exterior.emplace_back(corners[i], nullptr);
    }

    HalfEdge* prev_edge = nullptr;
    HalfEdge* first_corner_edge = nullptr;
    int first_origin_id = -1;
    for (auto [vertex, edge_out] : exterior) {
        RealCoordinate* vertex_coord = &exterior_vertices[next_vertex_id++];
        *vertex_coord = vertex;
        vertices.push_back(vertex_coord);
        HalfEdge* new_edge = &boundary_half_edges[next_half_edge_id++];
        HalfEdge* twin_edge = &boundary_half_edges[next_half_edge_id++];
        if (edge_out == nullptr) {
            //new_edge->region = nullptr;
            new_edge->prev = prev_edge;
            new_edge->origin_id = vertices.size() - 1;
            new_edge->twin = twin_edge;
            twin_edge->twin = new_edge;
            if (first_corner_edge == nullptr) { 
                first_corner_edge = new_edge; 
                first_origin_id = vertices.size() - 1;    
            }
            if (prev_edge) {
                prev_edge->next = new_edge;
                prev_edge->twin->origin_id = vertices.size() - 1;
            }
            prev_edge = new_edge;
            half_edges.push_back(new_edge);
            half_edges.push_back(twin_edge);
        }
        else {
            edge_out->origin_id = vertices.size() - 1;
            new_edge->region = edge_out->twin->region;
            new_edge->prev = edge_out->twin;
            new_edge->origin_id = vertices.size() - 1;
            new_edge->twin = twin_edge;
            twin_edge->twin = new_edge;
            if (first_corner_edge == nullptr) { 
                first_corner_edge = new_edge; 
                first_origin_id = vertices.size() - 1;    
            }
            if (prev_edge) { 
                prev_edge->next = edge_out; 
                prev_edge->twin->origin_id = vertices.size() - 1;
                edge_out->prev = prev_edge;    
            }
            prev_edge = new_edge;
            edge_out->twin->next = prev_edge;
            half_edges.push_back(prev_edge);
            half_edges.push_back(twin_edge);
        }
    }
    HalfEdge* edge_out = exterior.begin()->second;
    if (edge_out == nullptr) {
        first_corner_edge->prev = prev_edge;
        prev_edge->next = first_corner_edge;
        prev_edge->twin->origin_id = first_origin_id;
    }
    else {
        HalfEdge* next = edge_out;
        prev_edge->next = next;
        prev_edge->twin->origin_id = first_origin_id;
        next->prev = prev_edge;
    }
}
    
RealCoordinate Calculator::clip_infinite_edge(
    Impl::HalfEdge* edge, const Bbox& bounds
) {
    const auto& [x0, y0] = *vertices[edge->twin->origin_id];
    const auto& [rx1, ry1] = edge->region->seed;
    const auto& [rx2, ry2] = edge->twin->region->seed;
    double y_int, x_int;
    if (rx1 == rx2) {
        double rx3 = edge->next->twin->region->seed.x;
        x_int = (rx3 - rx1) * (x0 - rx1) > 0 ? bounds.xmax : bounds.xmin;
        y_int = 0.5 * (ry1 + ry2);
    }
    else if (ry1 == ry2) {
        // TODO figure this case out properly
        x_int = 0.5 * (rx1 + rx2);
        y_int = y0 > ry1 ? bounds.ymin : bounds.ymax;
    }
    else {
        RealCoordinate mid = {0.5 * (rx1 + rx2), 0.5 * (ry1 + ry2)};
        double m = (y0 - mid.y) / (x0 - mid.x);
        double b = y0 - m * x0;
        x_int = ry2 > ry1 ? bounds.xmax : bounds.xmin;
        y_int = m * x_int + b;
        if (y_int < bounds.ymin) {
            y_int = bounds.ymin;
            x_int = (y_int - b) / m;
        }
        else if (y_int > bounds.ymax) {
            y_int = bounds.ymax;
            x_int = (y_int - b) / m;
        }
    }
    return {x_int, y_int};

}

std::vector<RealCoordinate> Calculator::generate_seeds(
    int nseeds, const Bbox& box
) {
    std::uniform_real_distribution<double> random_x(box.xmin, box.xmax);
    std::uniform_real_distribution<double> random_y(box.ymin, box.ymax);
    std::vector<RealCoordinate> seeds;
    std::unordered_set<RealCoordinate> already_added;
    seeds.reserve(nseeds);
    already_added.reserve(nseeds);
    while (seeds.size() < nseeds) {
        RealCoordinate c(random_x(rng), random_y(rng));
        if (already_added.find(c) == already_added.end()) {
            seeds.push_back(c);
            already_added.insert(c);
        }
        else {
            std::cout << "Avoided duplicate" << std::endl;
        }
    }
    //std::cout << "Seeds size is: " << seeds.size() << std::endl;
    return seeds;
}

std::vector<RealCoordinate> Calculator::region_centroids() {
    using namespace Impl;
    std::vector<RealCoordinate> centroids; centroids.reserve(regions.size());
    std::vector<RealCoordinate> region_vertices; region_vertices.reserve(8);
    for (Region* region : regions) {
        HalfEdge* edge = region->an_edge;
        region_vertices.push_back(*vertices[edge->origin_id]);
        while (edge->next != region->an_edge) {
            edge = edge->next;
            region_vertices.push_back(*vertices[edge->origin_id]);
        }
        centroids.push_back(polygon_centroid(region_vertices));
        // clear de-allocates, if optimizing, create a manager class
        region_vertices.clear(); region_vertices.reserve(8);
    }
    return centroids;
}
    
}