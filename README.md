# VoronoiDiagram
A C++ library for generating and working with voronoi diagrams. 

Applies Fortune's Algorithm, [wiki link](https://en.wikipedia.org/wiki/Fortune%27s_algorithm), for computing the diagram for a desired number of seed points in O(nlog(n)) time. Employs a DCEL representation internally, [wiki link](https://en.wikipedia.org/wiki/Doubly_connected_edge_list), and provides methods for computing and returning the vertex or region graph as desired by the client.

# Get started

The library can be fetched conveniently via CMake:

```cmake
include(FetchContent)
FetchContent_Declare(
    VoronoiDiagram
    GIT_REPOSITORY https://github.com/MattKAnderson/VoronoiDiagram
    GIT_TAG main
)
FetchContent_MakeAvailable(VoronoiDiagram)
```

After fetching it can be linked to any target like any other library:

```cmake
target_link_libraries(MyTarget VoronoiDiagram::VoronoiDiagram)
```

In your source files only 1 header needs to be included:

```cpp
#include <VoronoiDiagram/VoronoiDiagram.hpp>
```

# API
The API exposes 7 classes. 

| class | Brief |
|-|-|
| `VoronoiDiagram::Calculator` | computes diagram, relaxes diagram, exposes vertex/region graph results |
| `VoronoiDiagram::VertexGraph` | Holds and owns the vertex graph data |
| `VoronoiDiagram::VertexNode` | A node representing a vertex in the diagram | 
| `VoronoiDiagram::RegionGraph` | Holds and owns the region graph |
| `VoronoiDiagram::RegionNode` | Node representing one of the diagram's regions |
| `VoronoiDiagram::Bbox` | A struct for specifying bounds |
| `VoronoiDiagram::RealCoordinate` | A struct representing a 2-d floating point coordinate |

Sample usage:

```cpp
int seed = 19212
int nsites = 100;
VoronoiDiagram::calculator calc(seed);
VoronoiDiagram::Bbox bounds{0.0, 2048.0, 0.0, 2048.0};

// randomly generate 100 sites within bounds, generate the
// diagram, and bound the diagram to bounds. This method is
// overloaded to allow passing in the sites rather than
// generating them 
calc.generate(nsites, bounds);

// relax the diagram, and bound it to bounds
for (int i = 0; i < 5; ++i) {
  calc.relax(bounds);
}

// produce and return the vertex graph (different from
// internal representation used for computation) 
VoronoiDiagram::VertexGraph = calc.get_vertex_graph();

// do what you want with the vertex graph...
```

# Examples

Examples can be found under the `example/` directory. Examples include producing and plotting the vertex graph, both the vertex and region graphs, plotting regions 1 by 1, relaxing the diagram for a specified number of iterations

# Sample results
<p align="center">
  <img src=data/vertex_graph.png alt=vertex_graph width=600>
  <img src=data/dual_graph.png alt=dual_graph width=600>
  <img src=data/voronoi_relaxation.gif alt=voronoi_relaxation width=600>
</p>

# Performance
The following performance metrics were recorded on a machine with an Intel i5-13600kf at 5.1 Ghz and 3600 Mhz CL16 DDR4 ram.

### Time to generate 
| Number of sites (regions) | Average of 10 runs (ms) |
| - | - |
| 100 | 0.130 |
| 1000 | 1.221 |
| 10000 | 13.210 | 
| 100000 | 157.524 |
| 500000 | 909.726 |

### Time to generate and relax 5 times
| Number of sites (regions) | Average of 10 runs (ms) |
| - | - |
| 100 | 0.739 |
| 1000 | 7.040 |
| 10000 | 77.223 |
| 100000 | died |
| 500000 | died |

### Time to get VertexGraph
| Number of sites (regions) | Average of 5 runs (ms) |
| - | - |
| 100 | 0.007 |
| 1000 | 0.081 |
| 10000 | 0.848 |
| 100000 | 8.885 |
| 500000 | 46.256 |

### Time to get RegionGraph
| Number of sites (regions) | Average of 5 runs (ms) |
| - | - |
| 100 | 0.013 |
| 1000 | 0.207 |
| 10000 | 2.114 |
| 100000 | 25.238 |
| 500000 | 123.517 |

TODO: produce graph and compare with other implementations, e.g. boost

# Known Issues
- Numerical stability issues when number of seeds is high and Bbox is very small
