import os
import json
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


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


def plot(data, start, stop, tick_step=None):

    fig, ax = plt.subplots()
    fig.set_size_inches(12.0, 7)
    fig.set_dpi(100)
    ax.set_title("Time to generate diagram", fontsize=20, pad=10.0)
    ax.set_xlabel("Number of sites", fontsize=12, labelpad=10.0)
    ax.set_ylabel("Time (ms)", fontsize=12, labelpad=36.0, rotation=0.0)

    for name, curve in data.items():
       poi = [p for p in curve if p[0] >= start and p[0] <= stop]
       xs = [p[0] for p in poi]
       ys = [p[1] for p in poi]
       ax.plot(xs, ys, marker='o', label=name) 

    if (tick_step):
        ax.xaxis.set_major_locator(
            ticker.MultipleLocator(tick_step)
        )

    ax.legend(fontsize=12)
    xmin, xmax, _, ymax = plt.axis()
    plt.axis((xmin, xmax, 0.0, ymax))
    plt.xticks(rotation=30, fontsize=11)
    plt.yticks(fontsize=11)
    plt.subplots_adjust(bottom=0.15, top=0.9)
    plt.show()
    fig.savefig("performance_comparison_plot_{}_{}.png".format(start, stop))


def main():
    data = load_measurements()
    plot(data, 0.0, 2000.0, 200.0)
    plot(data, 2000.0, 20000.0, 2000.0)
    plot(data, 100000.0, 200000.0, 10000.0)


if __name__ == "__main__":
    main()