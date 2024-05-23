import json
import matplotlib.pyplot as plt

xmin = -512.0
ymin = -512.0
xmax = 512.0
ymax = 512.0
padding = 50.0

def plot_line_segments(ax, lines, color=None):
    for line in lines:
        xs = [p[0] for p in line]
        ys = [p[1] for p in line]
        if color:
            ax.plot(xs, ys, color=color)
        else:
            ax.plot(xs, ys)

base_dir = "build/example/dual_graph/"
f1 = open(base_dir + "vertex_line_segments.json")
f2 = open(base_dir + "region_adjacency_line_segments.json")
vertex_line_segments = json.load(f1)["vector"]
region_line_segments = json.load(f2)["vector"]

fig, ax = plt.subplots(1)
ax.set_xlim(xmin - padding, xmax + padding)
ax.set_ylim(ymin - padding, ymax + padding)
plot_line_segments(ax, vertex_line_segments, "r")
plot_line_segments(ax, region_line_segments, "b")
plt.title("Dual Graph of Voronoi Diagram\nproduced by 100 sites")
plt.show()