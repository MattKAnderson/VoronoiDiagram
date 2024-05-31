#include <fastjet/internal/Voronoi.hh>
#include <test/common/common.hpp>

std::vector<fastjet::VPoint> real_coordinates_to_vpoints(
    std::vector<VoronoiDiagram::RealCoordinate>& coords
) {
    std::vector<fastjet::VPoint> vpoints;
    for (auto& coord : coords) {
        vpoints.emplace_back(coord.x, coord.y);
    }
    return vpoints;
}

int main() {
    using namespace std::chrono;
    using namespace VoronoiDiagram::Test;
    
    int seed = high_resolution_clock::now().time_since_epoch().count();
    int num_samples = 20;
    int max_psize = 250000;
    std::string filename = "../../../data/fastjet_performance_measurements.json";
    std::mt19937_64 rng(seed);
    VoronoiDiagram::Bbox bounds{0.0, 4096.0, 0.0, 4096.0};

    
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
            auto fj_sites = real_coordinates_to_vpoints(sites);
            auto t1 = high_resolution_clock::now();
            fastjet::VoronoiDiagramGenerator generator;
            generator.generateVoronoi(&fj_sites, bounds.xmin, bounds.xmax, bounds.ymin, bounds.ymax);
            auto t2 = high_resolution_clock::now();
            times.push_back(duration_cast<nanoseconds>(t2-t1).count());
        }
        measurements[0].emplace_back(psize, avg(times) / 1e6);
    }
    std::cout << "Writing measurements to JSON file located at: " << filename
              << std::endl;
    measurements_to_json(filename, measurements, labels);
    std::cout << "Done\n";
}