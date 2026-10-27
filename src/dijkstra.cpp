#include "dijkstra.h"
#include <QHash>
#include <limits>
#include <algorithm>

using namespace std;

DijkstraResult runDijkstra(const Graph& graph, long long startId, long long destId) {
    auto startTime = chrono::high_resolution_clock::now();
    DijkstraResult result;

    if (!graph.nodes.contains(startId) || !graph.nodes.contains(destId)) {
        result.success = false;
        return result;
    }

    QHash<long long, double> distMap;
    QHash<long long, long long> parentMap;
    QHash<long long, QString> edgeNameMap;

    for (auto it = graph.nodes.constBegin(); it != graph.nodes.constEnd(); ++it) {
        distMap[it.key()] = numeric_limits<double>::infinity();
    }

    // Custom MinHeap Priority Queue (No STL <queue> or std::priority_queue)
    MinHeap<long long, double> pq;

    distMap[startId] = 0.0;
    pq.push(startId, 0.0);

    bool found = false;

    while (!pq.empty()) {
        auto topItem = pq.top();
        pq.pop();

        double currentDist = topItem.priority;
        long long u = topItem.value;

        if (currentDist > distMap[u]) continue;
        result.nodesExploredCount++;
        result.exploredNodesList.append(u);

        if (u == destId) {
            found = true;
            break;
        }

        auto it = graph.adjacency.find(u);
        if (it != graph.adjacency.end()) {
            for (const auto& edge : it.value()) {
                long long v = edge.to;
                double alt = currentDist + edge.dist;

                if (alt < distMap[v]) {
                    distMap[v] = alt;
                    parentMap[v] = u;
                    edgeNameMap[v] = edge.name;
                    pq.push(v, alt);
                }
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    result.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

    if (!found) {
        result.success = false;
        return result;
    }

    result.success = true;
    result.totalDistanceMeters = distMap[destId];

    long long curr = destId;
    while (curr != startId) {
        result.pathNodes.append(curr);
        if (edgeNameMap.contains(curr)) {
            result.pathEdgeNames.append(edgeNameMap[curr]);
        }
        auto pIt = parentMap.find(curr);
        if (pIt == parentMap.end()) break;
        curr = pIt.value();
    }
    result.pathNodes.append(startId);

    // Reverse path to start -> destination order
    QList<long long> revPath;
    for (int i = result.pathNodes.size() - 1; i >= 0; --i) {
        revPath.append(result.pathNodes[i]);
    }
    result.pathNodes = revPath;

    QList<QString> revEdgeNames;
    for (int i = result.pathEdgeNames.size() - 1; i >= 0; --i) {
        revEdgeNames.append(result.pathEdgeNames[i]);
    }
    result.pathEdgeNames = revEdgeNames;

    return result;
}
