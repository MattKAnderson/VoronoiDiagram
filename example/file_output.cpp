#include <example/file_output.hpp>

namespace VoronoiDiagram::ExampleIO {

std::vector<std::pair<RealCoordinate, RealCoordinate>> vertex_graph_to_line_segments(
    std::vector<VertexNode*> graph
) {
    std::cout << "Graph size: " << graph.size() << std::endl;
    std::vector<std::pair<RealCoordinate, RealCoordinate>> line_segments;
    std::unordered_set<VertexNode*> visited;
    std::vector<VertexNode*> stack;
    stack.push_back(graph[0]);
    while (stack.size() != 0) {
        VertexNode* node = stack.back(); stack.pop_back();
        visited.insert(node);
        for (VertexNode* edge : node->connected) {
            if (visited.find(edge) != visited.end()) {
                continue;
            }
            if (edge == nullptr) {
                std::cout << "Null edge detected" << std::endl;
            }
            line_segments.emplace_back(node->coord, edge->coord);
            stack.push_back(edge);
        }
    }
    std::cout << "Number of line segments: " << line_segments.size() << std::endl;
    return line_segments;
}

std::vector<std::pair<RealCoordinate, RealCoordinate>> region_adjacency_line_segments(
    RegionGraph& graph
) {
    std::vector<std::pair<RealCoordinate, RealCoordinate>> line_segments;
    std::unordered_set<RegionNode*> visited;
    for (RegionNode* region : graph.get_regions()) {
        for (RegionNode* adj_region : region->adjacent) {
            if (visited.find(adj_region) != visited.end()) { continue; }
            line_segments.emplace_back(region->centroid(), adj_region->centroid());
        }
        visited.insert(region);
    }
    return line_segments;
}

std::string to_json(const RealCoordinate& c) {
    return "[" + std::format("{}", c.x) + ", " + std::format("{}", c.y) + "]";
}

std::string to_json(const std::pair<RealCoordinate, RealCoordinate>& p) {
    return "[" + to_json(p.first) + ", " + to_json(p.second) + "]";
}

std::string to_json(RegionNode* p) {
    std::string centroid = to_json(p->centroid());
    std::string vertices_string = "[";
    for (const RealCoordinate& vertex : p->vertices) {
        vertices_string += to_json(vertex) + ", ";
    }
    vertices_string.pop_back(); 
    vertices_string.pop_back();
    vertices_string += "]";
    return "{\"centroid\": " + centroid + ", \"vertices\": " + vertices_string + "}";
}
} // namespace VoronoiDiagram::ExampleIO