#include <iostream>
#include <limits>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>
#include <example/file_output.hpp>
#include <VoronoiDiagram/VoronoiDiagram.hpp>
#include <VoronoiDiagram/Impl/geometry.hpp>

int main() {
    using namespace VoronoiDiagram;
    using namespace VoronoiDiagram::ExampleIO;
    int seed = 1616;
    int nsites = 1000;
    Bbox bounds{-512.0, 512.0, -512.0, 512.0};
    
    // Produce the Diagram
    Calculator calculator(seed);
    calculator.generate(nsites, bounds);
    RegionGraph rgraph = calculator.get_region_graph();
    std::vector<RegionNode*> regions = rgraph.get_regions();

    std::cout << "Regions size: " << regions.size() << std::endl;

    // Find the central region (centroid closest to 0, 0)
    int closest_id = 0;
    double closest_sq_distance = std::numeric_limits<double>::max();
    for (int i = 0; i < regions.size(); ++i) {
        RealCoordinate centroid = regions[i]->centroid();
        double sq_dist = centroid.x * centroid.x + centroid.y * centroid.y;
        if (sq_dist < closest_sq_distance) {
            closest_id = i;
            closest_sq_distance = sq_dist;
        }
    }
    std::cout << "closest_id: " << closest_id << std::endl;;

    // Do a BFS starting at the closest region, send them to json in the 
    // visit order
    std::unordered_set<RegionNode*> visited; 
    std::queue<RegionNode*> region_node_queue;
    std::vector<RegionNode*> bfs_order; bfs_order.reserve(regions.size());
    region_node_queue.push(regions[closest_id]);
    while (!region_node_queue.empty()) {
        RegionNode* region = region_node_queue.front(); region_node_queue.pop();
        bfs_order.push_back(region);
        for (RegionNode* adj_region : region->adjacent) {
            if (adj_region == nullptr) {
                std::cout << "Caught a nullptr" << std::endl;
            }
            else if (visited.find(adj_region) == visited.end()) {
                region_node_queue.push(adj_region);
                visited.insert(adj_region);
            }
        }
    }
    vector_output("regions.json", bfs_order);
    std::cout << "Wrote regions to JSON" << std::endl;
}