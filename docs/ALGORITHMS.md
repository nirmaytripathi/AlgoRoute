# Pathfinding Algorithms: Dijkstra's Algorithm vs A* Search

This document provides a technical mathematical breakdown of the shortest path algorithms implemented in **AlgoRoute**.

---

## 1. Dijkstra's Algorithm (Uniform Cost Search)

### A. Overview
Dijkstra's algorithm, published by Edsger W. Dijkstra in 1959, solves the single-source shortest path problem for a weighted graph with non-negative edge weights.

### B. Mathematical Formulation
Given a weighted graph $G = (V, E, W)$ and a start vertex $s \in V$:
- Maintain a tentative distance function $d: V \to [0, \infty]$.
- Initially: $d(s) = 0$ and $d(v) = \infty$ for all $v \neq s$.
- Maintain a Min-Priority Queue $Q$ initialized with $(0, s)$.
- At each step, extract vertex $u = \arg\min_{v \in Q} d(v)$.
- For each outgoing edge $(u, v) \in E$ with weight $w(u, v)$:
  $$\text{If } d(u) + w(u, v) < d(v) \implies d(v) = d(u) + w(u, v), \text{ parent}(v) = u$$

### C. Complexity Analysis
- **Time Complexity**: $O((|V| + |E|) \log |V|)$ using a Min-Heap / Priority Queue.
- **Space Complexity**: $O(|V|)$ to store distance and parent maps.

---

## 2. A* Algorithm (Heuristic-Guided Search)

### A. Overview
A* (Hart, Nilsson & Raphael, 1968) is an informed search algorithm that uses both the actual path cost $g(n)$ and a heuristic estimate $h(n)$ to direct search towards the target.

### B. Evaluation Function
$$f(n) = g(n) + h(n)$$
- $g(n)$: Exact accumulated road distance from start node $s$ to node $n$.
- $h(n)$: Estimated distance from node $n$ to destination node $t$.
- $f(n)$: Estimated total route distance passing through $n$.

### C. Heuristic Selection: Haversine Straight-Line Distance
To guarantee optimality (shortest path), the heuristic $h(n)$ MUST be **admissible** and **consistent**:
1. **Admissibility**: $h(n) \le h^*(n)$ for all $n$, where $h^*(n)$ is the true shortest road distance. Since straight-line geographical distance is always $\le$ actual road distance (triangle inequality), the Haversine formula is strictly admissible.
2. **Consistency (Monotonicity)**: $h(n) \le w(n, p) + h(p)$ for any neighbor $p$.

---

## 3. Empirical Performance Comparison (Haldwani Benchmark)

| Metric | Dijkstra's Algorithm | A* Algorithm | Performance Delta |
| :--- | :--- | :--- | :--- |
| **Search Strategy** | Unidirectional Radial Expansion | Target-Directed Cone Expansion | A* eliminates irrelevant search space |
| **Nodes Explored** | ~4,615 nodes | ~376 nodes | **91.9% reduction in searched nodes** |
| **Execution Time** | ~3.73 ms | ~1.62 ms | **56.5% execution speedup** |
| **Path Distance** | 7.20 km | 7.20 km | **Identical (Guaranteed Optimal)** |
