#include <iostream>
#include <vector>
#include <utility>
#include <example/file_output.hpp>
#include <VoronoiDiagram/VoronoiDiagram.hpp>

int main() {
    using namespace VoronoiDiagram;
    using namespace VoronoiDiagram::ExampleIO;
    int seed = 1616;
    int nsites = 100;
    Bbox box{-512.0, 512.0, -512.0, 512.0};
    Calculator calculator(seed);

    calculator.generate(nsites, box);

    VertexGraph vgraph = calculator.get_vertex_graph();
    RegionGraph rgraph = calculator.get_region_graph();

    typedef std::vector<std::pair<RealCoordinate, RealCoordinate>> ls_vector;
    ls_vector vertex_line_segments = vertex_graph_to_line_segments(vgraph.get_vertices());
    ls_vector region_adj_line_segments = region_adjacency_line_segments(rgraph);
    
    std::cout << "Writing results" << std::endl;
    vector_output("vertex_line_segments.json", vertex_line_segments);
    vector_output("region_adjacency_line_segments.json", region_adj_line_segments);
}
