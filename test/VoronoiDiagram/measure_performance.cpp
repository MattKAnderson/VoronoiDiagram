#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <VoronoiDiagram/VoronoiDiagram.hpp>
#include <test/common/common.hpp>

int main() {
    using namespace std::chrono;
    using namespace VoronoiDiagram::Test;

    int seed = high_resolution_clock::now().time_since_epoch().count();
    int num_samples = 20;
    int max_psize = 25000;
    //int relax_count = 5;
    std::string filename = "../../../data/performance_measurements.json";
    std::mt19937_64 rng(seed);
    VoronoiDiagram::Bbox bounds{0.0, 4096.0, 0.0, 4096.0};
    
    std::vector<std::vector<std::pair<int, double>>> measurements = {
        {}, {}, {}
    };
    std::vector<std::string> labels = {
        "generate diagram",
        "generate vertex graph",
        "generate region graph"
    };
    std::vector<std::pair<int, int>> starts_and_steps = {
        {100, 100}, {1000, 1000}, {10000, 10000}
    };
    std::vector<int> problem_sizes = generate_problem_sizes(starts_and_steps, max_psize);

    for (int psize : problem_sizes) {
        std::cout << "Timing for problem size: " << psize << std::endl;
        std::vector<std::vector<double>> times = {{}, {}, {}};
        for (int i = 0; i < num_samples; i++) {
            VoronoiDiagram::Calculator calc;
            auto sites = random_points(rng, bounds, psize);
            auto t1 = high_resolution_clock::now();
            calc.generate(sites, bounds);
            auto t2 = high_resolution_clock::now();
            auto vgraph = calc.get_vertex_graph();
            auto t3 = high_resolution_clock::now();
            auto rgraph = calc.get_region_graph();
            auto t4 = high_resolution_clock::now();
            times[0].push_back(duration_cast<nanoseconds>(t2-t1).count());
            times[1].push_back(duration_cast<nanoseconds>(t3-t2).count());
            times[2].push_back(duration_cast<nanoseconds>(t4-t3).count());
        }
        measurements[0].emplace_back(psize, avg(times[0]) / 1e6);
        measurements[1].emplace_back(psize, avg(times[1]) / 1e6);
        measurements[2].emplace_back(psize, avg(times[2]) / 1e6); 
    }
    std::cout << "Writing measurements to JSON file located at: " << filename
              << std::endl;
    measurements_to_json(filename, measurements, labels);
}