#include <test/common/common.hpp>

namespace VoronoiDiagram::Test {
double avg(std::vector<double>& vals) {
    return std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
}

double min(std::vector<double>& vals) {
    return *std::min_element(vals.begin(), vals.end());
}

double max(std::vector<double>& vals) {
    return *std::max_element(vals.begin(), vals.end());
}

int get_next_start(
    int index,
    std::vector<std::pair<int, int>>& starts_and_steps,
    int stop
) {
    if (index < starts_and_steps.size()) {
        return starts_and_steps[index].first;
    }
    else {
        return stop + 1;
    }
}

std::vector<int> generate_problem_sizes(
    std::vector<std::pair<int, int>>& starts_and_steps,
    int stop
) {
    std::vector<int> problem_sizes;
    int current = starts_and_steps[0].first;
    int step = starts_and_steps[0].second;
    int index = 1;
    int next_start = get_next_start(index, starts_and_steps, stop);
    while (current <= stop) {
        problem_sizes.push_back(current);
        if (current >= next_start) {
            step = starts_and_steps[index].second;
            next_start = get_next_start(index, starts_and_steps, stop);
        }
        current += step;
    }
    return problem_sizes;
}

std::vector<RealCoordinate> random_points(
    std::mt19937_64& rng, const Bbox& bounds, int count
) {
    std::uniform_real_distribution<double> x_dist(bounds.xmin, bounds.xmax);
    std::uniform_real_distribution<double> y_dist(bounds.ymin, bounds.ymax);
    std::vector<RealCoordinate> points;
    for (int i = 0; i < count; i++) {
        points.emplace_back(x_dist(rng), y_dist(rng));
    }
    return points;
}

void measurements_to_json(
    std::string filename,
    std::vector<std::vector<std::pair<int, double>>>& samples,
    std::vector<std::string>& labels
) {
    std::ofstream outfile;
    outfile.open(filename);
    outfile << "{\n";
    for (int i = 0; i < samples.size(); i++) {
        outfile << "  \"" << labels[i] << "\": [\n";
        for (int j = 0; j < samples[i].size(); j++) {
            outfile << "    [" << samples[i][j].first << ", " 
                    << samples[i][j].second << "]";
            if (j < samples[i].size() - 1) {
                outfile << ",";
            }
            outfile << "\n";
        }  
        outfile << "  ]";
        if (i < samples.size() - 1) {
            outfile << ",";
        }
        outfile << "\n";
    }
    outfile << "}";
}
} // namespace VoronoiDiagram::Test