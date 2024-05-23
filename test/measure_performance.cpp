#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <VoronoiDiagram/VoronoiDiagram.hpp>

double avg(std::vector<double>& vals) {
    return std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
}

double min(std::vector<double>& vals) {
    return *std::min_element(vals.begin(), vals.end());
}

int main() {
    using namespace std::chrono;

    int problem_size = 100000;
    int start_seed = high_resolution_clock::now().time_since_epoch().count();
    int num_samples = 10;
    int relax_count = 5;
    VoronoiDiagram::Bbox bounds{0.0, 4096.0, 0.0, 4096.0};
    std::vector<double> generate_samples;
    std::vector<double> generate_and_relax_samples;
    std::vector<double> vertex_graph_samples;
    std::vector<double> region_graph_samples;
    std::mt19937_64 rng(start_seed);
    std::uniform_int_distribution seed_dist(0, 100000000);

    for (int i = 0; i < num_samples; ++i) {
        VoronoiDiagram::Calculator calc(seed_dist(rng)); // random seed
        auto t1 = high_resolution_clock::now();
        calc.generate(problem_size, bounds);
        auto t2 = high_resolution_clock::now();
        for (int j = 0; j < relax_count; ++j) {
            //calc.relax(bounds);
        }
        auto t3 = high_resolution_clock::now();
        auto vgraph = calc.get_vertex_graph();
        auto t4 = high_resolution_clock::now();
        auto rgraph = calc.get_region_graph();
        auto t5 = high_resolution_clock::now();
        generate_samples.push_back(duration_cast<nanoseconds>(t2-t1).count());
        generate_and_relax_samples.push_back(duration_cast<nanoseconds>(t3-t1).count());
        vertex_graph_samples.push_back(duration_cast<nanoseconds>(t4-t3).count());
        region_graph_samples.push_back(duration_cast<nanoseconds>(t5-t4).count());
    }

    double generate_avg = avg(generate_samples);
    double generate_min = min(generate_samples);
    double generate_and_relax_avg = avg(generate_and_relax_samples);
    double generate_and_relax_min = min(generate_and_relax_samples);
    double vgraph_avg = avg(vertex_graph_samples);
    double vgraph_min = min(vertex_graph_samples);
    double rgraph_avg = avg(region_graph_samples);
    double rgraph_min = min(region_graph_samples);

    std::cout << "Average Timing results\n"
              << "Problem size:     " << problem_size << "\n"
              << "generate:         " << generate_avg / 1e6 << " ms\n"
              << "generate + relax: " << generate_and_relax_avg / 1e6 << " ms\n"
              << "get vgraph:       " << vgraph_avg / 1e6 << " ms\n"
              << "get rgraph:       " << rgraph_avg / 1e6 << " ms\n"; 
    std::cout << "Minimum Timing results\n"
              << "generate:         " << generate_min / 1e6 << " ms\n"
              << "generate + relax: " << generate_and_relax_min / 1e6 << " ms\n"
              << "get vgraph:       " << vgraph_min / 1e6 << " ms\n"
              << "get rgraph:       " << rgraph_min / 1e6 << " ms\n";  
}