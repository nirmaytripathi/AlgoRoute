# AlgoRoute: Optimal Route Finder (Qt 6 C++ Desktop Application)

[![Qt 6](https://img.shields.io/badge/Qt-6.11.1-green.svg)](#)
[![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#)
[![DSA Project](https://img.shields.io/badge/Project-Data%20Structures%20%26%20Algorithms-indigo.svg)](#)
[![Location](https://img.shields.io/badge/Location-Haldwani%2C%20Uttarakhand-emerald.svg)](#)

**AlgoRoute** is a native C++ Desktop GUI Application built using **Qt 6** (`C:\Qt\6.11.1\mingw_64`) for **Haldwani, Uttarakhand, India**. It allows users to select locations across Haldwani and calculates the optimal shortest route using custom **Dijkstra's Algorithm** and **A* Algorithm** operating on real-world OpenStreetMap (OSM) road network data.

---

## 🌟 Features

1. **Native Qt 6 Desktop User Interface**:
   - Modern dark desktop theme built with Qt Widgets (`QtWidgets`, `QtSvg`, `QtCore`, `QtGui`).
   - Left Sidebar Controls (`QComboBox`, `QPushButton`, `QSlider`, `QGroupBox`).
   - Multi-tab View: **Haldwani Route View**, **Dijkstra vs A* Benchmark Table**, and **DSA Theory Documentation**.
   - Real-time status bar (`QStatusBar`).

2. **Real Haldwani OpenStreetMap Network**:
   - **6,816 Nodes** (intersections and coordinate markers).
   - **12,571 Edges** (directed road segments weighted by Haversine distance in meters).
   - 100% connected graph component ensuring valid pathfinding between all locations.

3. **Custom C++ Pathfinding Algorithms**:
   - **Zero External API Dependency**: Dijkstra and A* pathfinding algorithms are fully implemented in native C++ using `std::priority_queue` and Haversine distance heuristics ($f(n) = g(n) + h(n)$).

4. **Algorithm Comparison & Benchmarking**:
   - Compares Dijkstra vs A* side-by-side on identical Haldwani routes.
   - Measures Distance (km), Nodes Explored, and Execution Time (ms).

---

## 📁 Project Directory Structure

```
D:\AlgoRoute\
├── AlgoRoute.exe              # Native Compiled Qt 6 C++ Desktop Application
├── build_qt.bat               # Compilation script using qmake and mingw32-make
├── src/
│   ├── main.cpp               # Qt Application entry point
│   ├── mainwindow.h & cpp     # Qt MainWindow GUI implementation & dark QSS stylesheet
│   ├── graph.h & cpp          # C++ Graph class, JSON loader & Haversine formula
│   ├── dijkstra.h & cpp       # C++ Dijkstra's algorithm (std::priority_queue)
│   ├── astar.h & cpp          # C++ A* algorithm (Haversine heuristic)
│   ├── map_view.html          # Embedded HTML map canvas
│   ├── AlgoRoute.pro          # Qt Qmake Project File
│   └── CMakeLists.txt         # CMake Build File
├── data/
│   ├── haldwani_graph.json    # Real Haldwani OSM road network graph (6,816 nodes, 12,571 edges)
│   └── preset_locations.json  # Haldwani landmark dictionary
├── docs/
│   ├── ARCHITECTURE.md        # Qt 6 Desktop Architecture specification
│   ├── ALGORITHMS.md          # Mathematical breakdown of Dijkstra vs A*
│   └── TEST_CASES.md          # Verified Haldwani test suite
└── README.md                  # Project documentation
```

---

## 🚀 How to Run & Build

### 1. Launching the Compiled Application
Double-click `D:\AlgoRoute\AlgoRoute.exe` or run from PowerShell:
```powershell
D:\AlgoRoute\AlgoRoute.exe
```

### 2. Compiling from Source
Run the included build script in Command Prompt / PowerShell:
```cmd
cd D:\AlgoRoute
build_qt.bat
```

---

## 📊 Benchmark Results (Haldwani Railway Station -> Kathgodam Railway Station)

| Metric | Dijkstra's Algorithm | A* Algorithm | Advantage |
| :--- | :--- | :--- | :--- |
| **Total Distance** | **7.20 km** | **7.20 km** | **Identical (Guaranteed Optimal)** |
| **Nodes Explored** | **4,615 nodes** | **376 nodes** | **A* (91.9% fewer nodes explored)** |
| **Execution Time** | **4.09 ms** | **1.70 ms** | **A* (58.4% faster execution)** |
