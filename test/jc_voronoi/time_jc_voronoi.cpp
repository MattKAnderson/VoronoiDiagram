#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <test/common/common.hpp>
#include <jc_voronoi.h>


jcv_point* real_coordinates_to_jcv_points(
    std::vector<VoronoiDiagram::RealCoordinate>& coords
) {
    int size = coords.size();
    jcv_point* points = new jcv_point[size];
    for (int i = 0; i < size; i++) {
        points[i].x = coords[i].x;
        points[i].y = coords[i].y;
    }
    return points;
}


int main() {
    using namespace std::chrono;
    using namespace VoronoiDiagram::Test;

    int seed = high_resolution_clock::now().time_since_epoch().count();
    
    /*
     *  The jc_voronoi implementation seems to have a robustness issue
     *  where it will hang on some inputs. Keep num samples and max psize
     *  low to avoid this issue
     */
    int num_samples = 20;
    int max_psize = 25000;
    
    std::string filename = "../../../data/jc_voronoi_performance_measurements.json";
    std::mt19937_64 rng(seed);
    VoronoiDiagram::Bbox bounds{0.0, 4096.0, 0.0, 4096.0};
    jcv_rect jcv_bounds{{0.0, 0.0}, {4096.0, 4096.0}};
    
    std::vector<std::vector<std::pair<int, double>>> measurements = {{}};
    std::vector<std::string> labels = {"generate diagram"};
    std::vector<std::pair<int, int>> starts_and_steps = {
        {100, 100}, {1000, 1000}, {10000, 10000}
    };
    std::vector<int> problem_sizes = generate_problem_sizes(
        starts_and_steps, max_psize
    );

    for (int psize : problem_sizes) {
        std::cout << "Timing for problem size: " << psize << std::endl;
        std::vector<double> times;
        for (int i = 0; i < num_samples; i++) {
            auto sites = random_points(rng, bounds, psize);
            jcv_point* jcv_sites = real_coordinates_to_jcv_points(sites);
            auto t1 = std::chrono::high_resolution_clock::now();
            jcv_diagram diagram = {0};
            jcv_diagram_generate(psize, jcv_sites, &jcv_bounds, nullptr, &diagram);
            auto t2 = std::chrono::high_resolution_clock::now();
            times.push_back(duration_cast<nanoseconds>(t2-t1).count());
            jcv_diagram_free(&diagram);
        }
        measurements[0].emplace_back(psize, avg(times) / 1e6);
    }
    std::cout << "Writing measurements to JSON file located at: " << filename
              << std::endl;
    measurements_to_json(filename, measurements, labels);
    std::cout << "Done\n";
}