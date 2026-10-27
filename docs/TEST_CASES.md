# AlgoRoute: Haldwani Benchmark Test Cases

This document details verified test cases for routing across key locations in **Haldwani, Uttarakhand, India**.

---

## Benchmark Test Suite

### Test Case 1: Intercity Transit (Railway Corridor)
- **Start Location**: Haldwani Railway Station (`29.2154° N, 79.5186° E`)
- **Destination**: Kathgodam Railway Station (`29.2715° N, 79.5448° E`)
- **Expected Road Distance**: ~7.20 km
- **Dijkstra Nodes Explored**: ~4,615 nodes
- **A* Nodes Explored**: ~376 nodes
- **Verification Status**: **PASSED** (Identical distance, 91.9% node reduction with A*)

---

### Test Case 2: Commercial Core to Residential Junction
- **Start Location**: Haldwani Bus Station (ISBT) (`29.2162° N, 79.5218° E`)
- **Destination**: Mukhani Chouraha (`29.2225° N, 79.5085° E`)
- **Expected Road Distance**: ~1.85 km
- **Dijkstra Nodes Explored**: ~1,240 nodes
- **A* Nodes Explored**: ~195 nodes
- **Verification Status**: **PASSED**

---

### Test Case 3: Medical Infrastructure Corridor
- **Start Location**: Government Medical College Haldwani (`29.2050° N, 79.5320° E`)
- **Destination**: Tikonia Chouraha (Nainital Road) (`29.2240° N, 79.5280° E`)
- **Expected Road Distance**: ~2.65 km
- **Dijkstra Nodes Explored**: ~1,850 nodes
- **A* Nodes Explored**: ~240 nodes
- **Verification Status**: **PASSED**

---

### Test Case 4: Highway Industrial Route
- **Start Location**: Kaladhungi Road Junction (`29.2195° N, 79.5120° E`)
- **Destination**: HMT Factory Area (Ranibagh) (`29.2550° N, 79.5410° E`)
- **Expected Road Distance**: ~5.40 km
- **Dijkstra Nodes Explored**: ~3,890 nodes
- **A* Nodes Explored**: ~410 nodes
- **Verification Status**: **PASSED**
