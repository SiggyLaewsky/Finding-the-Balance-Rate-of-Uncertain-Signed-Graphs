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

1. Building the project: in project directory run

```bash
mkdir build && cd build && cmake .. && make
```
2. Data generator is provided in `generator` folder. Graphs are automatically generated and saved in `data` directory inside the corresponding subdirectory (see more details in the corresponding `README.md`).

### Format of `eta_list`
Multiple lines, each containing the fraction of appended edges (not in percents!)

### Format of `p_factor`
Multiple lines, each containing a factor to all the edge existence probabilities in the graph.

