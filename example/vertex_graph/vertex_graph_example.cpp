#include <iostream>
#include <vector>
#include <utility>
#include <example/file_output.hpp>
#include <VoronoiDiagram/VoronoiDiagram.hpp>

int main() {
    using namespace VoronoiDiagram;
    using namespace VoronoiDiagram::ExampleIO;
    int seed = 182;
    int nsites = 100;
    Bbox box{0.0, 4096.0, 0.0, 4096.0};
    Calculator calculator(seed);

    calculator.generate(nsites, box);

    VertexGraph vgraph = calculator.get_vertex_graph();
    
    typedef std::vector<std::pair<RealCoordinate, RealCoordinate>> ls_vector;
    ls_vector vertex_line_segments = vertex_graph_to_line_segments(vgraph.get_vertices());
    std::cout << "Writing results" << std::endl;
    vector_output("vertex_line_segments.json", vertex_line_segments);
    std::cout << "Done" << std::endl;
}