import json
import matplotlib.pyplot as plt
import matplotlib.animation as animation

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


def draw_regions_animation(fig, ax, regions):
    
    line_segments_x = [p[0] for p in regions[0]["vertices"]]
    line_segments_y = [p[1] for p in regions[0]["vertices"]]
    # regions_plot = ax.plot(line_segments_x, line_segments_y, label="region contours")[0]

    artists = []
    for i in range(len(regions)):
        vertices = regions[i]["vertices"]
        xs = [p[0] for p in vertices] + [vertices[0][0]]
        ys = [p[1] for p in vertices] + [vertices[0][1]]
        if artists:
            artists.append(artists[-1] + ax.plot(xs, ys))
        else:
            artists.append(ax.plot(xs, ys))
        
    ani = animation.ArtistAnimation(fig=fig, artists=artists, interval=3)
    plt.show()


fig, ax = plt.subplots(1)
fig.suptitle("Drawing Voronoi Cells 1 by 1")
ax.set_xlim(xmin - padding, xmax + padding)
ax.set_ylim(ymin - padding, ymax + padding)

base_dir = "build/example/region_bfs_animation/"
f1 = open(base_dir + "regions.json")
regions = json.load(f1)["vector"]

draw_regions_animation(fig, ax, regions)