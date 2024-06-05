import json
import matplotlib.pyplot as plt

xmin = 0.0
ymin = 0.0
xmax = 0.00001
ymax = 0.00001
padding = 0.000001

def plot_line_segments(ax, lines, color=None):
    for line in lines:
        xs = [p[0] for p in line]
        ys = [p[1] for p in line]
        if color:
            ax.plot(xs, ys, color=color)
        else:
            ax.plot(xs, ys)

def plot_seeds(ax, seeds, color="b"):
    xs = [point[0] for point in seeds]
    ys = [point[1] for point in seeds]
    ax.scatter(xs, ys, color=color)

base_dir = "build/example/vertex_graph/"
f1 = open(base_dir + "vertex_line_segments.json")
f2 = open(base_dir + "diagram_seeds.json")
line_segments = json.load(f1)["vector"]
seeds = json.load(f2)["vector"]

fig, ax = plt.subplots(1)
ax.set_xlim(xmin - padding, xmax + padding)
ax.set_ylim(ymin - padding, ymax + padding)
plot_line_segments(ax, line_segments, "r")
plot_seeds(ax, seeds)
plt.title("Voronoi Diagram produced from 1000 sites")
plt.show()