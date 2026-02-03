# Subdirectory Structure
```text
generator/
├── CMakeLists.txt                  # Build configuration
├── generator.h 
├── generator.cpp
├── nearly_balanced_sizes           # List of parameters for nearly-balanced graph generation
├── README.md                       # Subdirectory overview and usage
├── sparse_sizes                    # List of parameters for sparse graph generation
├── topology.h
├── topology.cpp                       
```
# File format
## `nearly_balanced_sizes`
The file consists of multiple lines, each describing a synthetic nearly-balanced graph.
In each graph, all vertices are partitioned into exactly two subsets.
A small number of cross-edges may be injected to perturb the perfectly balanced structure.
Each line follows the format:
1. Size of the first vertex subset
2. Size of the second vertex subset
3. Number of edges within the first subset 
4. Number of edges within the second subset
5. Number of edges between the two subsets
6. Number of negative edges within the first subset
7. Number of negative edges within the second subset

By construction, edges within subsets are positive, while edges between subsets are negative.

## `sparse_sizes`
The file consists of multiple line, each line is a number of vertices in a synthetic sparse graph.

# Build
`generator` is a separate target which is automatically built by the root `CMakeLists.txt`.

# Run

Generator takes one argument: a string, taking a value `nb` for generation the nearly balanced graphs or `sp` for generation the sparse highly-decomposable graphs. The parameters are taken from files
`nearly_balanced_sizes` and `sparse_sizes` respectively. Generated graphs are saved in directories 
`../data/synthetic_nearly_balanced` and `../data/synthetic/sparse` respectively. For more details about the format see in the corresponding `README.md`.