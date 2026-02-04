//
// Created by HighlySkilledCoder on 2026/1/23.
//

#ifndef BR_USG_DSU_H
#define BR_USG_DSU_H

#include <vector>

/**
 * @brief Disjoint Set Union with parity relations for signed edges.
 *
 * rel[u] stores the parity between u and parent[u] (false = same side,
 * true = flipped). The sign parameter in unite() uses the convention:
 *   sign == true  => positive edge (same side)
 *   sign == false => negative edge (different sides)
 */
struct DSU {
    /// Initialize DSU for n nodes.
    explicit DSU(int n);
    /// Reset the structure to the initial state.
    void reset();
    /**
     * @brief Find the root of u.
     * @return {parity_to_root, root}
     */
    std::pair<bool, int> find(int u);
    /**
     * @brief Unite u and v with a signed constraint.
     * @return true if the constraint is consistent; false if it conflicts.
     */
    bool unite(int u, int v, bool sign);
    std::vector<int> parent;  ///< Parent pointers.
    std::vector<bool> rel;  ///< Parity to parent.
    std::vector<int> rank;  ///< Union-by-rank heuristic.
};

#endif //BR_USG_DSU_H
