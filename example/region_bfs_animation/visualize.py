import json
import matplotlib.pyplot as plt

xmin = -512.0
ymin = -512.0
xmax = 512.0
ymax = 512.0
padding = 50.0

def plot_region(ax, region, bounds_color="r", centroid_color="b"):
    xs = [p[0] for p in region["vertices"]] + [region["vertices"][0][0]]
    ys = [p[1] for p in region["vertices"]] + [region["vertices"][0][1]]
    ax.plot(xs, ys, color=bounds_color)
    ax.scatter(region["centroid"][0], region["centroid"][1], color=centroid_color)


fig, ax = plt.subplots(1)
base_dir = "build/example/region_bfs_animation/"
f1 = open(base_dir + "regions.json")
regions = json.load(f1)["vector"]

for region in regions:
    plot_region(ax, region)

ax.set_xlim()
ax.set_ylim()
plt.show()