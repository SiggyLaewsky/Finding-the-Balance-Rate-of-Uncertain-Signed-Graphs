# File format
Any file except for `real_world_orig` has the following format:

The first line contains two integers:
- number of nodes
- number of edges

Each subsequent line describes one edge in the format:

- `head`  `tail`  `sign`  `probability`

where:
- `head`, `tail` are integer vertex IDs
- `sign` is `1` for a positive edge and `0` for a negative edge
- `probability` is the probability of edge existence

Vertices are numerated consequently starting from zero. 
Files in `real_world_orig` DO NOT have `probability` field in edge description!