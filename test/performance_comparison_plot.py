import os
import json
import matplotlib.pyplot as plt


JC_VORONOI_MEASUREMENTS_PATH = "../data/jc_voronoi_performance_measurements.json"
FASTJET_MEASUREMENTS_PATH = "../data/fastjet_performance_measurements.json"
BOOST_MEASUREMENTS_PATH = "../data/boost_performance_measurements.json"
VORONOI_DIAGRAM_MEASUREMENTS_PATH = "../data/performance_measurements.json"

def load_measurements():
    script_path = os.path.dirname(os.path.realpath(__file__))
    jc_path = os.path.join(script_path, JC_VORONOI_MEASUREMENTS_PATH)
    fj_path = os.path.join(script_path, FASTJET_MEASUREMENTS_PATH)
    b_path = os.path.join(script_path, BOOST_MEASUREMENTS_PATH)
    vd_path = os.path.join(script_path, VORONOI_DIAGRAM_MEASUREMENTS_PATH)
    jc_measurements = json.load(open(jc_path))
    fj_measurements = json.load(open(fj_path))
    b_measurements = json.load(open(b_path))
    vd_measurements = json.load(open(vd_path))
    return {
        #"jc_voronoi": jc_measurements["generate diagram"],
        "fastjet": fj_measurements["generate diagram"],
        "boost (no clipping included)": b_measurements["generate diagram"],
        "VoronoiDiagram": vd_measurements["generate diagram"]
    }


def main():
    data = load_measurements()

    fig, ax = plt.subplots()
    ax.set_title("Compute time comparison")
    ax.set_xlabel("Number of sites")
    ax.set_ylabel("Time (ms)")

    for name, curve in data.items():
       xs = [point[0] for point in curve]
       ys = [point[1] for point in curve]
       ax.plot(xs, ys, marker='o', label=name) 

    ax.legend()
    plt.show()
    fig.savefig("performance_comparison_plot.png")


if __name__ == "__main__":
    main()