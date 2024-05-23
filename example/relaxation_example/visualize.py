import json
import matplotlib.pyplot as plt
import matplotlib.animation as ani

n_relaxation_rounds = 25
xmin = -512.0
ymin = -512.0
xmax = 512.0
ymax = 512.0
padding = 50.0

"""
def relax_vertex_graph_animation(fig, ax, arr_of_arr_of_lines, color="r"):
    artists = []

    for arr_of_lines in arr_of_arr_of_lines:
        matplotlib_lines = []
        for line in arr_of_lines:
            xs =[p[0] for p in line]
            ys = [p[1] for p in line]
            matplotlib_lines.extend(ax.plot(xs, ys, color=color))
        artists.append(matplotlib_lines)

    # repeat the initial frame 3 times
    artists = [artists[0], artists[0]] + artists

    animation = ani.ArtistAnimation(fig=fig, artists=artists, interval=100)
    plt.show()
"""

def relax_vertex_graph_animation(fig, ax, arr_of_arr_of_lines, color="r"):
    plt.suptitle(
        "Relaxing a Voronoi Diagram over " 
        + str(n_relaxation_rounds) + " iterations"
    )
    n_frames = n_relaxation_rounds + 1
    titles = ["Iteration " + str(i) for i in range(n_frames)]
    #xs = [p[0] for arr in arr_of_arr_of_lines[0] for p in arr]
    #ys = [p[1] for arr in arr_of_arr_of_lines[0] for p in arr]
    lines = []
    for line in arr_of_arr_of_lines[0]:
        xs = [p[0] for p in line]
        ys = [p[1] for p in line]
        lines.extend(ax.plot(xs, ys, color=color))
    
    def update(frame):
        #plt.title(titles[frame])
        for i, line in enumerate(arr_of_arr_of_lines[frame]):
            xs = [p[0] for p in line]
            ys = [p[1] for p in line]
            lines[i].set_data(xs, ys)
        return lines
    
    def init():
        ax.set_xlim(xmin - padding, xmax + padding)
        ax.set_ylim(ymin - padding, ymax + padding)
    
    animation = ani.FuncAnimation(fig, update, frames=n_frames, init_func=init, interval=100)
    plt.show()
    animation.save("voronoi_relaxation.gif", writer="pillow")
        

base_dir = "build/example/relaxation_example/"
base_name = "vertex_line_segments_"
vertex_line_segments = []
for i in range(n_relaxation_rounds + 1):
    f = open(base_dir + base_name + str(i) + ".json")
    vertex_line_segments.append(json.load(f)["vector"])

fig, ax = plt.subplots(1)
ax.set_xlim(xmin - padding, xmax + padding)
ax.set_ylim(ymin - padding, ymax + padding)
relax_vertex_graph_animation(fig, ax, vertex_line_segments, "r")