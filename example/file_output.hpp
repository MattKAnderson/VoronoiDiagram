#pragma once
#include <iostream>
#include <fstream>
#include <format>
#include <vector>
#include <utility>
#include <unordered_set>
#include <VoronoiDiagram/Coordinate.hpp>
#include <VoronoiDiagram/VoronoiDiagram.hpp>

namespace VoronoiDiagram::ExampleIO {

std::vector<std::pair<RealCoordinate, RealCoordinate>> vertex_graph_to_line_segments(
    std::vector<VertexGraph::Node*> graph
); 
std::vector<std::pair<RealCoordinate, RealCoordinate>> region_adjacency_line_segments(
    RegionGraph& graph
);
std::string to_json(const RealCoordinate& c);
std::string to_json(const std::pair<RealCoordinate, RealCoordinate>& p);
std::string to_json(RegionGraph::Node* p);
template <typename T>
void vector_output(std::string filename, std::vector<T>& vec) {
    std::ofstream outfile;
    outfile.open(filename);
    outfile << "{\"vector\": [";
    for (int i = 0; i < vec.size() - 1; i++) {
        outfile << to_json(vec[i]) << ", ";
    }
    outfile << to_json(vec.back()) << "]}";
}

} // namespace VoronoiDiagram::ExampleIO