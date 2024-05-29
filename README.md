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
| 100 | 0.074 |
| 1000 | 0.709 |
| 10000 | 7.355 | 
| 100000 | 87.229 |
| 500000 | 530.652 |

### Time to generate and relax 5 times
| Number of sites (regions) | Average of 10 runs (ms) |
| - | - |
| 100 | 0.391 |
| 1000 | 3.901 |
| 10000 | 42.097 |
| 100000 | 485.658 |
| 500000 | 2803.48 |

### Time to get VertexGraph
| Number of sites (regions) | Average of 5 runs (ms) |
| - | - |
| 100 | 0.007 |
| 1000 | 0.081 |
| 10000 | 0.794 |
| 100000 | 8.168 |
| 500000 | 43.3628 |

### Time to get RegionGraph
| Number of sites (regions) | Average of 5 runs (ms) |
| - | - |
| 100 | 0.013 |
| 1000 | 0.207 |
| 10000 | 1.920 |
| 100000 | 22.272 |
| 500000 | 120.373 |

# Known Issues
- Numerical stability issues when number of seeds is high and Bbox is very small

# TODO

- produce graph and compare with other implementations, e.g. boost
- use enahnced numerical precision, e.g. 128 bits, when it is detected it is needed (during if checks where the test values are within 64-bit epsilon)
    - check this resolves errors due to numerical precision when the sites are extremely densely packed
- sanitize inputs (ensure no duplicate site points)
- Write validation test script that asserts the Voronoi diagram properties are held true after producing the diagram (e.g. number of connections each vertice has, that the closest 3 sites are equidistant from vertex in question, no edge crosses another edge, etc).  
- Write a robustness test script that only tests successful completion for millions of seeds
- Further micro-optimization / memory management
