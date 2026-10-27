#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <cmath>
#include <QList>
#include <QHash>
#include <QString>
#include "datastructures.h"

using namespace std;

struct Edge {
    long long to;
    double dist;
    QString name;
    QString type;
};

struct Node {
    long long id;
    double lat;
    double lon;
    QString name;
};

class Graph {
public:
    QHash<long long, Node> nodes;
    QHash<long long, QList<Edge>> adjacency;

    Graph() = default;

    // Load graph JSON
    bool loadFromJSON(const QString& filename);

    // Haversine geographic distance in meters
    static double haversine(double lat1, double lon1, double lat2, double lon2);

    // Spatial lookup to find nearest graph node
    long long findNearestNode(double lat, double lon) const;
};

#endif // GRAPH_H
