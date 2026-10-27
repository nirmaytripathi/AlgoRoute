# AlgoRoute: Optimal Route Finder (Qt 6 C++ Desktop Application)

[![Qt 6](https://img.shields.io/badge/Qt-6.11.1-green.svg)](#)
[![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#)
[![DSA Project](https://img.shields.io/badge/Project-Data%20Structures%20%26%20Algorithms-indigo.svg)](#)
[![Location](https://img.shields.io/badge/Location-Haldwani%2C%20Uttarakhand-emerald.svg)](#)

**AlgoRoute** is a native C++ Desktop GUI Application built using **Qt 6** for **Haldwani, Uttarakhand, India**. It allows users to select locations across Haldwani and calculates the optimal shortest route using custom **Dijkstra's Algorithm** and **A* Algorithm** operating on real-world OpenStreetMap (OSM) road network data.

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
AlgoRoute/
├── src/
│   ├── main.cpp, mainwindow.h/.cpp, mapwidget.h/.cpp
│   ├── graph.h/.cpp, dijkstra.h/.cpp, astar.h/.cpp, datastructures.h
│   ├── resources.qrc          # Embeds the JSON data inside the executable
│   ├── AlgoRoute.pro          # qmake project
│   └── CMakeLists.txt         # CMake project
├── forms/mainwindow.ui
├── data/                      # Haldwani OSM graph + landmark presets
├── docs/
└── README.md
```

---

## 🚀 Requirements

- **Qt 6** (Widgets, Svg, Network modules) installed, 6.2 or newer
- A C++17 compiler (MinGW / MSVC / GCC / Clang)
- Internet connection (map tiles are downloaded at runtime)

Works on Windows, Linux and macOS. The road-network data is embedded in the executable, so no data paths need configuring.

## 🔨 Build & Run

### Option A: Qt Creator (easiest)
Open `src/CMakeLists.txt` (or `src/AlgoRoute.pro`), pick your Qt 6 kit, press **Run**.

### Option B: Command line (CMake)
```bash
cmake -S src -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/<compiler>
cmake --build build
./build/AlgoRoute        # build\AlgoRoute.exe on Windows
```

### Option C: Command line (qmake)
```bash
cd src
qmake AlgoRoute.pro
make            # mingw32-make on Windows with MinGW
```

---

## 📊 Benchmark Results (Haldwani Railway Station -> Kathgodam Railway Station)

| Metric | Dijkstra's Algorithm | A* Algorithm | Advantage |
| :--- | :--- | :--- | :--- |
| **Total Distance** | **7.20 km** | **7.20 km** | **Identical (Guaranteed Optimal)** |
| **Nodes Explored** | **4,615 nodes** | **376 nodes** | **A* (91.9% fewer nodes explored)** |
| **Execution Time** | **4.09 ms** | **1.70 ms** | **A* (58.4% faster execution)** |
