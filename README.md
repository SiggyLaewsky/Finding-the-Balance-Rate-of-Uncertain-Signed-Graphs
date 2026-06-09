# Finding-the-Balance-Rate-of-Uncertain-Signed-Graphs
Implementation &amp; numerical experiments.

## Project Structure

```text
Finding-the-Balance-Rate-of-Uncertain-Signed-Graphs/
├── README.md                       # Project overview and usage
├── CMakeLists.txt                  # Build configuration
├── core/                           # Algorithms
├── data/                           # Data directory
│   └── real_world_balanced         # Extracted balanced subgraphs from real graphs
│   └── real_world_orig             # Real graphs
│   └── synthetic_nearly_balanced   # Synthetic graphs: balanced with small injections
│   └── synthetic_sparse            # Synthetic graphs: highly decomposable
├── generator/                      # Subdirectory for generating the synthetic data
├── stat/                           # Computations of statistical characteristics + plots
├── eta_list                        # Parameters for the second experiment
├── p_factor_list                   # Parameters for the second experiment
└── main.cpp
```

## Code Overview

- `core/graph.*`: Signed graph storage, file IO, and preprocessing into components.
- `core/dsu.*`: Union-find with parity for signed constraints.
- `core/algo.*`: Monte Carlo balance estimators and greedy edge-removal search.
- `experiments.*`: Experiment drivers used by `main.cpp`.
- `generator/`: Synthetic data generator (`BR_GENERATOR`) and topology helpers.
- `stat/`: Statistics, confidence intervals, and plotting via embedded Python.
- `utils.*`: Parameter parsing and directory listing helpers.

## CLI Usage

Main binary (`BR_USG`):

```
-i  <1|2|3|4>   Experiment id (efficiency, p/eta sweep, cross-edge, br vs ebc)
-ns <int>       Number of samples per balance-rate evaluation
-nt <int>       Number of OpenMP threads
-d  <path>      Output directory for plots (optional)
```

Generator binary (`BR_GENERATOR`):

```
BR_GENERATOR sp   # sparse synthetic graphs (sizes from generator/sparse_sizes)
BR_GENERATOR nb   # nearly balanced graphs (params from generator/nearly_balanced_sizes)
```

## Graph File Format

```
n m
head tail sign prob
...
```

Each edge line stores integer endpoints, sign as 0/1 (1 = positive), and the
Bernoulli mean `prob` for edge existence.

1. Building the project: in project directory run

```bash
mkdir build && cd build && cmake .. && make
```
2. Data generator is provided in `generator` folder. Graphs are automatically generated and saved in `data` directory inside the corresponding subdirectory (see more details in the corresponding `README.md`).

### Format of `eta_list`
Multiple lines, each containing the fraction of appended edges (not in percents!)

### Format of `p_factor`
Multiple lines, each containing a factor to all the edge existence probabilities in the graph.
