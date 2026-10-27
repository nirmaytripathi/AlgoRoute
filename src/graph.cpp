#include "graph.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <limits>
#include <algorithm>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double Graph::haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000.0; // Earth radius in meters
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);

    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return R * c;
}

long long Graph::findNearestNode(double lat, double lon) const {
    double minDist = numeric_limits<double>::max();
    long long nearestId = -1;

    for (auto it = nodes.constBegin(); it != nodes.constEnd(); ++it) {
        double dist = haversine(lat, lon, it.value().lat, it.value().lon);
        if (dist < minDist) {
            minDist = dist;
            nearestId = it.key();
        }
    }
    return nearestId;
}

bool Graph::loadFromJSON(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "Error: Could not open graph file " << filename.toStdString() << endl;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        cerr << "Error: Invalid JSON format in " << filename.toStdString() << endl;
        return false;
    }

    QJsonObject root = doc.object();
    QJsonObject nodesObj = root["nodes"].toObject();
    QJsonObject adjObj = root["adjacency"].toObject();

    nodes.clear();
    adjacency.clear();

    // 1. Load Nodes with 100% Exact Coordinates
    for (auto it = nodesObj.begin(); it != nodesObj.end(); ++it) {
        long long id = it.key().toLongLong();
        QJsonObject nObj = it.value().toObject();

        Node n;
        n.id = id;
        n.lat = nObj["lat"].toDouble();
        n.lon = nObj["lon"].toDouble();
        if (nObj.contains("name") && !nObj["name"].isNull()) {
            n.name = nObj["name"].toString();
        }
        nodes[id] = n;
    }

    // 2. Load Adjacency Edges
    for (auto it = adjObj.constBegin(); it != adjObj.constEnd(); ++it) {
        long long fromId = it.key().toLongLong();
        const QJsonArray edgesArr = it.value().toArray();

        for (const QJsonValue &eVal : std::as_const(edgesArr)) {
            QJsonObject eObj = eVal.toObject();
            Edge edge;
            edge.to = eObj["to"].toVariant().toLongLong();
            edge.dist = eObj["dist"].toDouble();
            edge.name = eObj.contains("name") && !eObj["name"].isNull() ? eObj["name"].toString() : "Haldwani Road";
            edge.type = "road";
            adjacency[fromId].append(edge);
        }
    }

    cout << "Qt Graph Successfully Loaded: " << nodes.size() << " nodes, " << adjacency.size() << " adjacency lists." << endl;
    return true;
}
