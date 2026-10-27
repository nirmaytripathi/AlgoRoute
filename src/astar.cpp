#include "astar.h"
#include <QHash>
#include <limits>
#include <algorithm>

using namespace std;

AStarResult runAStar(const Graph& graph, long long startId, long long destId) {
    auto startTime = chrono::high_resolution_clock::now();
    AStarResult result;

    if (!graph.nodes.contains(startId) || !graph.nodes.contains(destId)) {
        result.success = false;
        return result;
    }

    const Node& destNode = graph.nodes.value(destId);

    QHash<long long, double> gScore;
    QHash<long long, double> fScore;
    QHash<long long, long long> parentMap;
    QHash<long long, QString> edgeNameMap;

    for (auto it = graph.nodes.constBegin(); it != graph.nodes.constEnd(); ++it) {
        gScore[it.key()] = numeric_limits<double>::infinity();
        fScore[it.key()] = numeric_limits<double>::infinity();
    }

    // Custom MinHeap Priority Queue (No STL <queue> or std::priority_queue)
    MinHeap<long long, double> pq;

    const Node& startNode = graph.nodes.value(startId);
    double initialH = Graph::haversine(startNode.lat, startNode.lon, destNode.lat, destNode.lon);

    gScore[startId] = 0.0;
    fScore[startId] = initialH;
    pq.push(startId, initialH);

    bool found = false;

    while (!pq.empty()) {
        auto topItem = pq.top();
        pq.pop();

        long long u = topItem.value;

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
                double tentativeG = gScore[u] + edge.dist;

                if (tentativeG < gScore[v]) {
                    parentMap[v] = u;
                    edgeNameMap[v] = edge.name;
                    gScore[v] = tentativeG;

                    const Node& neighborNode = graph.nodes.value(v);
                    double hVal = Graph::haversine(neighborNode.lat, neighborNode.lon, destNode.lat, destNode.lon);
                    double fVal = tentativeG + hVal;
                    fScore[v] = fVal;

                    pq.push(v, fVal);
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
    result.totalDistanceMeters = gScore[destId];

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
