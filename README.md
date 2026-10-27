# 🧭 AlgoRoute: Optimal Route Finder for Haldwani

[![Qt 6](https://img.shields.io/badge/Qt-6.11.1-green.svg)](#)
[![Language](https://img.shields.io/badge/C++-17-blue.svg)](#)
[![DSA Project](https://img.shields.io/badge/Project-Data%20Structures%20%26%20Algorithms-indigo.svg)](#)
[![Location](https://img.shields.io/badge/Location-Haldwani%2C%20Uttarakhand-emerald.svg)](#)

**AlgoRoute** is a native C++ desktop application built with **Qt 6** that finds the shortest route between locations in **Haldwani, Uttarakhand, India**. It runs custom implementations of **Dijkstra's Algorithm** and **A\* Search** on a real-world **OpenStreetMap (OSM)** road network, and compares the two side by side.

> 📸 *Add a screenshot of the Route View tab here: `docs/screenshot-route-view.png`*

---

## 📑 Table of Contents

1. [Features](#-features)
2. [How It Works](#-how-it-works)
3. [Benchmark Results](#-benchmark-results)
4. [Project Structure](#-project-structure)
5. [Requirements](#-requirements)
6. [Build & Run](#-build--run)
7. [Usage](#-usage)
8. [Team & Work Distribution](#-team--work-distribution)
9. [Future Scope](#-future-scope)

---

## 🌟 Features

- **Native Qt 6 desktop UI** with a modern dark theme (`QtWidgets`, `QtSvg`, `QtCore`, `QtGui`).
- **Sidebar controls**: source and destination selection (`QComboBox`), action buttons (`QPushButton`), sliders (`QSlider`) and grouped options (`QGroupBox`).
- **Three tabs**:
  - **Haldwani Route View**: visualize the computed route on the map.
  - **Dijkstra vs A\* Benchmark Table**: side-by-side comparison on the same route.
  - **DSA Theory Documentation**: in-app explanation of the algorithms and data structures.
- **Real OSM data of Haldwani**:
  - **6,816 nodes** (intersections and coordinate markers)
  - **12,571 edges** (directed road segments, weighted by Haversine distance in meters)
  - The graph is a single connected component, so a valid path exists between all locations.
- **Custom pathfinding in native C++**: no external routing API. Both algorithms use `std::priority_queue`, and A\* uses the Haversine distance as its heuristic, `f(n) = g(n) + h(n)`.
- **Benchmarking**: reports distance (km), nodes explored and execution time (ms) for both algorithms.
- **Real-time status bar** (`QStatusBar`) for feedback.

---

## ⚙️ How It Works

1. **Graph loading**: the OSM road network is stored as JSON, embedded in the executable via `resources.qrc`, and loaded into an adjacency-list graph at startup.
2. **Route request**: the user picks a source and destination from the landmark presets.
3. **Dijkstra**: expands nodes in increasing order of `g(n)` (distance from source) using a min-priority queue. It guarantees the shortest path but explores widely.
4. **A\***: expands nodes by `f(n) = g(n) + h(n)`, where `h(n)` is the Haversine (straight-line) distance to the destination. The heuristic is admissible, so A\* is also optimal, but it explores far fewer nodes.
5. **Visualization**: the resulting path is drawn on the map widget, and the metrics are added to the benchmark table.

---

## 📊 Benchmark Results

Route: **Haldwani Railway Station → Kathgodam Railway Station**

| Metric             | Dijkstra's Algorithm | A\* Algorithm | Advantage                            |
|:-------------------|:---------------------|:--------------|:-------------------------------------|
| **Total Distance** | 7.20 km              | 7.20 km       | Identical (both guaranteed optimal)  |
| **Nodes Explored** | 4,615 nodes          | 376 nodes     | A\* explores 91.9% fewer nodes       |
| **Execution Time** | 4.09 ms              | 1.70 ms       | A\* is 58.4% faster                  |

---

## 📁 Project Structure

```
AlgoRoute/
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── mainwindow.h / .cpp      # Main window, tabs, sidebar logic
│   ├── mapwidget.h / .cpp       # Map rendering and route drawing
│   ├── graph.h / .cpp           # Graph + OSM data loading
│   ├── dijkstra.h / .cpp        # Dijkstra's algorithm
│   ├── astar.h / .cpp           # A* algorithm + Haversine heuristic
│   ├── datastructures.h         # Shared structs / helper types
│   ├── resources.qrc            # Embeds JSON data in the executable
│   ├── AlgoRoute.pro            # qmake project
│   └── CMakeLists.txt           # CMake project
├── forms/
│   └── mainwindow.ui            # Qt Designer layout
├── data/                        # Haldwani OSM graph + landmark presets
├── docs/                        # Project documentation
└── README.md
```

---

## 🚀 Requirements

- **Qt 6.2 or newer** (Widgets, Svg and Network modules)
- A **C++17** compiler (MinGW / MSVC / GCC / Clang)
- **Internet connection**: map tiles are downloaded at runtime

Works on Windows, Linux and macOS. The road-network data is embedded in the executable, so no data paths need to be configured.

---

## 🔨 Build & Run

### Option A: Qt Creator (easiest)

Open `src/CMakeLists.txt` (or `src/AlgoRoute.pro`), select your Qt 6 kit, and press **Run**.

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
make                     # mingw32-make on Windows with MinGW
```

---

## 🖱️ Usage

1. Launch the app.
2. Choose a **source** and a **destination** from the sidebar.
3. Click the route button to compute the path.
4. View the route on the **Haldwani Route View** tab.
5. Open the **Benchmark** tab to compare Dijkstra and A\*.
6. Open the **DSA Theory** tab to read how the algorithms work.

---

## 👥 Team & Work Distribution

| Member | Role | Responsibilities | Files / Areas |
|:-------|:-----|:-----------------|:--------------|
| **Member 1** *Nirmay Tripathi* | **Data & Graph Engineer** | Collect and clean the Haldwani OSM data. Build the graph (nodes, directed edges, Haversine weights). Verify the graph is fully connected. Define landmark presets. | `data/`, `graph.h/.cpp`, `datastructures.h`, `resources.qrc` |
| **Member 2** *Tushar Tiwari* | **Algorithm Developer** | Implement Dijkstra with `std::priority_queue`. Implement A\* with the Haversine heuristic. Return the path, distance and nodes-explored count. Prove or verify optimality. | `dijkstra.h/.cpp`, `astar.h/.cpp` |
| **Member 3** *Suraj Pandey* | **GUI Developer** | Design the dark theme and the main window. Build the sidebar controls and tabs. Draw the map and route in the map widget. Wire the UI to the algorithms. | `mainwindow.h/.cpp`, `mapwidget.h/.cpp`, `forms/mainwindow.ui`, `main.cpp` |
| **Member 4** *Vaibhav Bhatt* | **Benchmarking, Testing & Documentation** | Build the benchmark table (distance, nodes, time). Test routes across many source/destination pairs. Maintain the CMake and qmake builds. Write the DSA Theory tab, `docs/` and this README. | Benchmark module, `CMakeLists.txt`, `AlgoRoute.pro`, `docs/`, `README.md` |

### 🤝 Shared responsibilities

- **Code reviews**: every pull request is reviewed by at least one other member.
- **Integration**: Members 2 and 3 coordinate on the interface between the algorithms and the UI. Members 1 and 2 coordinate on the graph API.
- **Testing**: every member tests their own module before merging into `main`.

### 🔀 Git workflow

- `main` holds the stable, working version.
- Each member works on a feature branch, for example `feature/graph`, `feature/algorithms`, `feature/gui`, `feature/benchmark`.
- Use clear commit messages and merge through pull requests.

---

## 🔮 Future Scope

- Support for more cities and larger OSM extracts
- Turn-by-turn directions and estimated travel time
- Additional algorithms (Bidirectional Dijkstra, ALT, Contraction Hierarchies)
- Road-type-aware weights (traffic, speed limits)
- Offline map tiles

---
