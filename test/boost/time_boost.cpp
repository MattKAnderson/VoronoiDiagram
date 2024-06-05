#include <test/common/common.hpp>
#include <boost/polygon/voronoi.hpp>

std::vector<boost::polygon::point_data<double>> rc_to_boost_points(
    std::vector<VoronoiDiagram::RealCoordinate>& coords
) {
    std::vector<boost::polygon::point_data<double>> points;
    for (auto& c : coords) {
        points.emplace_back(c.x, c.y);
    }
    return points;
}

int main() {
    using namespace std::chrono;
    using namespace VoronoiDiagram::Test;
    using namespace boost::polygon;

    int seed = high_resolution_clock::now().time_since_epoch().count();
    int num_samples = 50;
    int max_psize = 250000;
    std::string filename = "../../../data/boost_performance_measurements.json";
    std::mt19937_64 rng(seed);
    VoronoiDiagram::Bbox bounds{0.0, 4096.0, 0.0, 4096.0};
    
    std::vector<std::vector<std::pair<int, double>>> measurements = {{}};
    std::vector<std::string> labels = {"generate diagram"};
    std::vector<std::pair<int, int>> starts_and_steps = {
        {100, 100}, {1000, 1000}, {10000, 10000}
    };
    std::vector<int> problem_sizes = generate_problem_sizes(starts_and_steps, max_psize);

    for (int psize : problem_sizes) {
        std::cout << "Timing for problem size: " << psize << std::endl;
        std::vector<double> times;
        for (int i = 0; i < num_samples; i++) {
            auto sites = random_points(rng, bounds, psize);
            auto boost_sites = rc_to_boost_points(sites);
            auto t1 = high_resolution_clock::now();
            voronoi_diagram<double> diagram;
            construct_voronoi(boost_sites.begin(), boost_sites.end(), &diagram);
            auto t2 = high_resolution_clock::now();
            times.push_back(duration_cast<nanoseconds>(t2-t1).count());
        }
        measurements[0].emplace_back(psize, avg(times) / 1e6);
    }
    std::cout << "Writing measurements to JSON file located at: " << filename
              << std::endl;
    measurements_to_json(filename, measurements, labels);




}