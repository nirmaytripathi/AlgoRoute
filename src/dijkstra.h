#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "datastructures.h"
#include <QList>
#include <QString>
#include <chrono>

using namespace std;

struct DijkstraResult {
    bool success = false;
    double totalDistanceMeters = 0.0;
    size_t nodesExploredCount = 0;
    double executionTimeMs = 0.0;
    QList<long long> pathNodes;
    QList<long long> exploredNodesList;
    QList<QString> pathEdgeNames;
};

DijkstraResult runDijkstra(const Graph& graph, long long startId, long long destId);

#endif // DIJKSTRA_H
