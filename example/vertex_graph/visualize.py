import json
import matplotlib.pyplot as plt

xmin = 0.0
ymin = 0.0
xmax = 4096
ymax = 4096
padding = 100.0

def plot_line_segments(ax, lines, color=None):
    for line in lines:
        xs = [p[0] for p in line]
        ys = [p[1] for p in line]
        if color:
            ax.plot(xs, ys, color=color)
        else:
            ax.plot(xs, ys)


base_dir = "build/example/"
f1 = open(base_dir + "vertex_line_segments.json")
line_segments = json.load(f1)["vector"]

fig, ax = plt.subplots(1)
ax.set_xlim(xmin - padding, xmax + padding)
ax.set_ylim(ymin - padding, ymax + padding)
plot_line_segments(ax, line_segments, "r")
plt.show()