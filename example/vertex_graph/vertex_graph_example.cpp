#include <chrono>
#include <iostream>
#include <vector>
#include <utility>
#include <example/file_output.hpp>
#include <VoronoiDiagram/VoronoiDiagram.hpp>

int main() {
    using namespace VoronoiDiagram;
    using namespace VoronoiDiagram::ExampleIO;
    int nsites = 1000;
    Bbox box{0.0, 0.00001, 0.0, 0.00001};
    
    int seed = 228241;
    //int seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    Calculator calculator(seed);
    calculator.generate(nsites, box);
    VertexGraph vgraph = calculator.get_vertex_graph();
    
    typedef std::vector<std::pair<RealCoordinate, RealCoordinate>> ls_vector;
    ls_vector vertex_line_segments = vertex_graph_to_line_segments(vgraph.get_vertices());
    std::vector<RealCoordinate> seeds = calculator.get_seeds();
    std::cout << "Writing results" << std::endl;
    vector_output("vertex_line_segments.json", vertex_line_segments);
    vector_output("diagram_seeds.json", seeds);
    std::cout << "Done" << std::endl;
}