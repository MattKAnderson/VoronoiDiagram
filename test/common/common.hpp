#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <utility>
#include <VoronoiDiagram/Bbox.hpp>
#include <VoronoiDiagram/Coordinate.hpp>

namespace VoronoiDiagram::Test {
double avg(std::vector<double>& vals);
double min(std::vector<double>& vals);
double max(std::vector<double>& vals);
std::vector<int> generate_problem_sizes(
    std::vector<std::pair<int, int>>& starts_and_steps,
    int stop
);
std::vector<RealCoordinate> random_points(
    std::mt19937_64& rng, const VoronoiDiagram::Bbox& bounds, int count
);
void measurements_to_json(
    std::string filename,
    std::vector<std::vector<std::pair<int, double>>>& samples,
    std::vector<std::string>& labels
);
} // namespace VoronoiDiagram