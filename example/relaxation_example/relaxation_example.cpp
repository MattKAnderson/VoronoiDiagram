#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <example/file_output.hpp>
#include <VoronoiDiagram/VoronoiDiagram.hpp>

int main() {
    using namespace VoronoiDiagram;
    using namespace VoronoiDiagram::ExampleIO;
    typedef std::vector<std::pair<RealCoordinate, RealCoordinate>> ls_vector;
    int seed = 918;
    int nsites = 1000;
    Bbox bounds{-512.0, 512.0, -512.0, 512.0};
    VoronoiDiagram::Calculator calculator(seed); 

    calculator.generate(nsites, bounds);
    
    std::vector<ls_vector> vertex_line_segments;
    VertexGraph vgraph = calculator.get_vertex_graph();
    vertex_line_segments.push_back(vertex_graph_to_line_segments(vgraph.get_vertices()));
    for (int i = 0; i < 25; i++) {
        calculator.relax(bounds);
        VertexGraph vgraph = calculator.get_vertex_graph();
        vertex_line_segments.push_back(vertex_graph_to_line_segments(vgraph.get_vertices()));
    }

    std::cout << "Writing results" << std::endl;
    for (int i = 0; i < vertex_line_segments.size(); ++i) {
        std::string filename = "vertex_line_segments_" + std::to_string(i) + ".json";
        vector_output(filename, vertex_line_segments[i]);
    }
}