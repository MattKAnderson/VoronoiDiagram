# VoronoiDiagram
A simple, easy to use, performant, and robust C++ library for generating and working with voronoi diagrams. 

<p align="center">
  <img src=data/dual_graph.png alt=dual_graph width=400>
  <img src=data/voronoi_relaxation.gif alt=voronoi_relaxation width=400>
</p>

This library aims to be highly usable, consumed by users who want a convenient API and a simple install for a performant, lightweight, and robust implementation that does more than just return the minimum viable datastructure. This library will return immediately usable datastructures that manage their heap allocated memory for you. The diagram is clipped and bounded by the library, and the dual graphs (vertex / region graphs) are constructed as objects containing a graph of nodes with adjacency lists. No need to interpret an unbounded doubly-connected edge list yourself.

# Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Note on Robustness and Correctness](#note-on-robustness-and-correctness)
- [Performance](#performance)
- [Roadmap](#roadmap)

# Installation

It is recommended to use the tool [CMake](https://cmake.org/documentation/) to build your project and to fetch this library. 

By adding a few lines to the CMakeLists.txt at the root of your project, CMake will download and make this library available within the project:

```cmake
include(FetchContent)
FetchContent_Declare(
    VoronoiDiagram
    GIT_REPOSITORY https://github.com/MattKAnderson/VoronoiDiagram
    GIT_TAG main
)
FetchContent_MakeAvailable(VoronoiDiagram)
```

After fetching it can be linked to any target in the project just like any other library:

```cmake
target_link_libraries(MyTarget VoronoiDiagram::VoronoiDiagram)
```

Linking the library to the target automatically makes the library headers available to that target.

# Usage

In your source files only 1 header needs to be included:

```cpp
#include <VoronoiDiagram/VoronoiDiagram.hpp>
```

All objects exist within the `VoronoiDiagram` namespace:

```cpp
VoronoiDiagram::calculator calc;
VoronoiDiagram::Bbox bounds;
```

The voronoi diagram is produced by the `calculator` object. It contains a `generate` method for producing the diagram. This method is overloaded so that the user can supply a number of sites or the actual sites used to generate the graph as well as either the bounds to use or no bounds (which implies bounding at infinite edges to the max x/y values of any vertex in the diagram).

```cpp
// the calculator needs to be seeded, if none is provided 0 is used as a default
// the prng is used for generating the sites if `generate` is called with a number
int seed = 12911;
VoronoiDiagram::calculator voronoi_calculator(seed);

// The Bbox object can be used for specifying bounds
VoronoiDiagram::Bbox bounds(0.0, 2048.0, 0.0, 2048.0);

// Generate the diagram for 100 random sites within bounds
voronoi_calculator.generate(100, bounds);

// Generate the diagram for 100 sites defined in some vector and bound to bounds
std::vector<VoronoiDiagram::RealCoordinate> sites = { {12.0, 14.0}, ... } // some vector of 100 predefined sites
voronoi_calculator.generate(sites, bounds);

// use the same sites as before but bound to the minimum bbox containing all intersect points
voronoi_calculator.generate(sites);
```

Note: Each call to `generate` resets the internal state of the calculator, then calculates the diagram for the given input. For the above code, the final state of the calculator would be that of the diagram produced by the final call to `generate`. In this way a calculator can be re-used to compute many diagrams.

The diagram can be relaxed before returning results using the calculator's `relax` method

```cpp
// relax the diagram one time (i.e. compute centroids, and use these as sites in another diagram generation)
voronoi_calculator.relax()

// relax many times, note TODO
voronoi_calculator.relax_iters(10);
```

The two main results to fetch are the Vertex and Region graphs. These graphs are only produced when the get method is called and each time the get method is called.

```cpp 
VoronoiDiagram::VertexGraph vgraph = voronoi_calculator.get_vertex_graph();

VoronoiDiagram::RegionGraph rgraph = voronoi_calculator.get_region_graph();
```

Both `RegionGraph` and `VertexGraph` contain nodes of the graph internally and have accessors to return a pointer to a node in the graph or a vector of all nodes. The `RegionNode` and `VertexNode` themselves differ. `VertexNode`s are simple, containing a coordinate and an adjacency list. `RegionNode` contains the site, it's CCW ordered vertices, and an adjacency list. The `RegionNode` additionally contains methods for computing the centroid and area.

```cpp
// example finding the largest region
RegionNode* largest_region = nullptr;
double largest_area = std::numeric_limits<double>::min();
std:vector<RegionNode*> regions = rgraph.get_regions();
for (RegionNode* region : regions) {
    double region_area = region->area();
    if (region_area > largest_area) {
        largest_area = region_area;
        largest_region = region;
    }
}
```

# Examples

Examples can be found under the `example/` directory. Examples include producing and plotting the vertex graph, both the vertex and region graphs, plotting regions 1 by 1, relaxing the diagram for a specified number of iterations

# Note on Robustness and Correctness

Correctness in computational geometry is very difficult due to numerical in-precision. Algorithms that are theoretically correct can be practically incorrect due to the in-precision of floating point numbers. In many use cases this in-precision means the answer is "a little off", `12.0000000000001` and `12.0` is only a slight difference. However in computational geometry it is the topological structure that matters and "being behind" instead of "ahead of" when you should be "ahead of" is just plain wrong, not "a little off". 

This library does not currently address numerical in-precision issues, nor does it address the co-circular degeneracy case (where > 3 sites lie on the circumference of the same circle). So it is not correct for any input case. However, at this time this library will robustly compute the diagrams produced by millions of random seed sites. This is sufficient for many use cases which do not require perfect correctness, and is likely sufficient for the intended users of this library. 

Addressing these limitations is on the roadmap. If correctness is the chief concern then check out [CGAL](https://www.cgal.org/).

# Performance
## Comparison
VoronoiDiagram is a competitive implementation in comparison to leading implementations. The Boost implementation is a likely the most 
The time to generate and clip the voronoi diagram for specified number of pre-generated sites is plotted below for VoronoiDiagram, fastjet, and Boost. Note: Boost does not clip the diagram and the additional time that would take is not captured in the measurements of the Boost implementation

<p align="center">
  <img src=data/performance_comparison_plot_0.0_1000.0.png alt=performance_comparison_plot_0_to_1000, width=800> 
  <img src=data/performance_comparison_plot_1000.0_100000.0.png alt=performance_comparison_plot_1000_to_100000, width=800> 
</p>

## Operation times
The following table summarizes the avg. time over 20 runs in milliseconds (ms) to complete 4 core operations for different problem sizes

| Operation | 100 sites | 1000 sites | 10,000 sites | 100,000 sites |
| - | - | - | - | - |
| Generate | 0.074 | 0.709 | 7.355 | 87.229 |
| Relax 5x | | | | |
| Get VertexGraph | 0.007 | 0.081 | 0.794 | 8.168 |
| Get RegionGraph | 0.013 | 0.207 | 1.920 | 22.272 |

# Roadmap

- investigate using a [bucket queue](https://en.wikipedia.org/wiki/Bucket_queue), since it is the PQ consume largest share of runtime
- use enahnced numerical precision when it is detected it is needed (during if checks where the test values are within 64-bit epsilon)
    - check this resolves errors due to numerical precision when the sites are extremely densely packed
- Write validation test script (e.g. test cell areas sum to total bbox area, errors in diagram will result discrepancy)
- Write a robustness test script that only tests successful completion for millions of seeds
- Further micro-optimization / memory management
