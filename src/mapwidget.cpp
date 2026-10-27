#include "mapwidget.h"
#include <QPainterPath>
#include <QToolTip>
#include <QUrl>
#include <QDebug>
#include <cmath>
#include <algorithm>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

HaldwaniMapWidget::HaldwaniMapWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 450);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_tileCache.setMaxCost(400);
    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, &QNetworkAccessManager::finished, this, &HaldwaniMapWidget::onTileDownloaded);

    // Setup Animation Timer
    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &HaldwaniMapWidget::onAnimStep);

    // Setup Dark Map Context Menu
    m_clickMenu = new QMenu(this);
    m_clickMenu->setStyleSheet(R"(
        QMenu {
            background-color: #0f172a;
            border: 1px solid #06b6d4;
            border-radius: 8px;
            padding: 4px;
            color: #f8fafc;
            font-family: 'Outfit', sans-serif;
            font-size: 13px;
        }
        QMenu::item {
            padding: 8px 16px;
            border-radius: 4px;
        }
        QMenu::item:selected {
            background-color: #06b6d4;
            color: #090d16;
            font-weight: bold;
        }
    )");

    m_setStartAction = m_clickMenu->addAction("🟢 Set as Starting Location");
    m_setDestAction = m_clickMenu->addAction("🔴 Set as Destination");

    connect(m_setStartAction, &QAction::triggered, this, &HaldwaniMapWidget::onSetStartTriggered);
    connect(m_setDestAction, &QAction::triggered, this, &HaldwaniMapWidget::onSetDestTriggered);
}

HaldwaniMapWidget::~HaldwaniMapWidget() {
}

void HaldwaniMapWidget::setGraph(const Graph *graph) {
    m_graph = graph;
    update();
}

void HaldwaniMapWidget::setStartLocation(long long nodeId, double lat, double lon, const QString &name) {
    m_startNodeId = nodeId;
    m_startLat = lat;
    m_startLon = lon;
    m_startName = name;
    m_hasStart = true;
    autoFitView();
}

void HaldwaniMapWidget::setDestLocation(long long nodeId, double lat, double lon, const QString &name) {
    m_destNodeId = nodeId;
    m_destLat = lat;
    m_destLon = lon;
    m_destName = name;
    m_hasDest = true;
    autoFitView();
}

void HaldwaniMapWidget::setStartNode(long long nodeId, const QString &name) {
    if (m_graph && m_graph->nodes.contains(nodeId)) {
        const Node &n = m_graph->nodes.value(nodeId);
        setStartLocation(nodeId, n.lat, n.lon, name);
    } else {
        m_startNodeId = nodeId;
        m_startName = name;
        m_hasStart = (nodeId != -1);
        autoFitView();
    }
}

void HaldwaniMapWidget::setDestNode(long long nodeId, const QString &name) {
    if (m_graph && m_graph->nodes.contains(nodeId)) {
        const Node &n = m_graph->nodes.value(nodeId);
        setDestLocation(nodeId, n.lat, n.lon, name);
    } else {
        m_destNodeId = nodeId;
        m_destName = name;
        m_hasDest = (nodeId != -1);
        autoFitView();
    }
}

void HaldwaniMapWidget::autoFitView() {
    QList<QPointF> points;
    if (m_hasStart) {
        points.append(QPointF(m_startLat, m_startLon));
    }
    if (m_hasDest) {
        points.append(QPointF(m_destLat, m_destLon));
    }

    if (points.isEmpty()) return;

    if (points.size() == 1) {
        m_centerLat = points[0].x();
        m_centerLon = points[0].y();
        m_zoom = 15;
    } else {
        double minLat = min(points[0].x(), points[1].x());
        double maxLat = max(points[0].x(), points[1].x());
        double minLon = min(points[0].y(), points[1].y());
        double maxLon = max(points[0].y(), points[1].y());

        m_centerLat = (minLat + maxLat) / 2.0;
        m_centerLon = (minLon + maxLon) / 2.0;

        double span = max(maxLat - minLat, maxLon - minLon);
        if (span > 0.10) m_zoom = 12;
        else if (span > 0.05) m_zoom = 13;
        else if (span > 0.02) m_zoom = 14;
        else m_zoom = 15;
    }

    update();
}

void HaldwaniMapWidget::setOptimalPath(const QList<long long> &pathNodes) {
    m_animTimer->stop();
    m_pathNodes = pathNodes;
    m_exploredNodes.clear();

    if (!m_pathNodes.isEmpty() && m_graph) {
        double minLat = 90.0, maxLat = -90.0;
        double minLon = 180.0, maxLon = -180.0;

        for (long long nid : m_pathNodes) {
            auto it = m_graph->nodes.find(nid);
            if (it != m_graph->nodes.end()) {
                minLat = min(minLat, it.value().lat);
                maxLat = max(maxLat, it.value().lat);
                minLon = min(minLon, it.value().lon);
                maxLon = max(maxLon, it.value().lon);
            }
        }

        if (m_hasStart) {
            minLat = min(minLat, m_startLat);
            maxLat = max(maxLat, m_startLat);
            minLon = min(minLon, m_startLon);
            maxLon = max(maxLon, m_startLon);
        }
        if (m_hasDest) {
            minLat = min(minLat, m_destLat);
            maxLat = max(maxLat, m_destLat);
            minLon = min(minLon, m_destLon);
            maxLon = max(maxLon, m_destLon);
        }

        if (minLat < maxLat && minLon < maxLon) {
            m_centerLat = (minLat + maxLat) / 2.0;
            m_centerLon = (minLon + maxLon) / 2.0;

            double span = max(maxLat - minLat, maxLon - minLon);
            if (span > 0.08) m_zoom = 12;
            else if (span > 0.04) m_zoom = 13;
            else if (span > 0.02) m_zoom = 14;
            else m_zoom = 15;
        }
    }

    update();
}

void HaldwaniMapWidget::startExplorationAnimation(const QList<long long> &exploredNodes, const QList<long long> &finalPath, int speedMultiplier) {
    m_animTimer->stop();

    m_fullExploredList = exploredNodes;
    m_finalPathNodes = finalPath;
    m_pathNodes.clear();
    m_exploredNodes.clear();
    m_animIndex = 0;

    if (m_fullExploredList.isEmpty()) return;

    int clampedSpeed = max(1, min(speedMultiplier, 10));
    m_animBatchSize = max(1, static_cast<int>(m_fullExploredList.size() * (clampedSpeed * 0.008)));

    m_animTimer->start(16);
}

void HaldwaniMapWidget::onAnimStep() {
    if (m_animIndex >= m_fullExploredList.size()) {
        m_animTimer->stop();
        m_pathNodes = m_finalPathNodes;
        update();
        return;
    }

    int endIdx = min(m_animIndex + m_animBatchSize, static_cast<int>(m_fullExploredList.size()));
    for (int i = m_animIndex; i < endIdx; ++i) {
        m_exploredNodes.append(m_fullExploredList[i]);
    }
    m_animIndex = endIdx;

    update();
}

void HaldwaniMapWidget::clearMap() {
    m_animTimer->stop();
    m_hasStart = false;
    m_hasDest = false;
    m_startNodeId = -1;
    m_destNodeId = -1;
    m_startName.clear();
    m_destName.clear();
    m_pathNodes.clear();
    m_exploredNodes.clear();
    m_fullExploredList.clear();
    m_finalPathNodes.clear();
    m_centerLat = 29.2188;
    m_centerLon = 79.5130;
    m_zoom = 14;
    update();
}

int HaldwaniMapWidget::lonToTileX(double lon, int z) {
    return static_cast<int>(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int HaldwaniMapWidget::latToTileY(double lat, int z) {
    double latRad = lat * M_PI / 180.0;
    return static_cast<int>(floor((1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * (1 << z)));
}

double HaldwaniMapWidget::tileXToLon(double x, int z) {
    return x / static_cast<double>(1 << z) * 360.0 - 180.0;
}

double HaldwaniMapWidget::tileYToLat(double y, int z) {
    double n = M_PI - 2.0 * M_PI * y / static_cast<double>(1 << z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

QPointF HaldwaniMapWidget::latLonToScreen(double lat, double lon) const {
    int n = 1 << m_zoom;
    double worldX = (lon + 180.0) / 360.0 * n * 256.0;
    double latRad = lat * M_PI / 180.0;
    double worldY = (1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * n * 256.0;

    double centerWorldX = (m_centerLon + 180.0) / 360.0 * n * 256.0;
    double centerLatRad = m_centerLat * M_PI / 180.0;
    double centerWorldY = (1.0 - log(tan(centerLatRad) + 1.0 / cos(centerLatRad)) / M_PI) / 2.0 * n * 256.0;

    double screenX = width() / 2.0 + (worldX - centerWorldX);
    double screenY = height() / 2.0 + (worldY - centerWorldY);

    return QPointF(screenX, screenY);
}

void HaldwaniMapWidget::screenToLatLon(const QPointF &screenPos, double &lat, double &lon) const {
    int n = 1 << m_zoom;
    double centerWorldX = (m_centerLon + 180.0) / 360.0 * n * 256.0;
    double centerLatRad = m_centerLat * M_PI / 180.0;
    double centerWorldY = (1.0 - log(tan(centerLatRad) + 1.0 / cos(centerLatRad)) / M_PI) / 2.0 * n * 256.0;

    double worldX = centerWorldX + (screenPos.x() - width() / 2.0);
    double worldY = centerWorldY + (screenPos.y() - height() / 2.0);

    lon = (worldX / 256.0 / n) * 360.0 - 180.0;
    double nVal = M_PI - 2.0 * M_PI * (worldY / 256.0 / n);
    lat = 180.0 / M_PI * atan(0.5 * (exp(nVal) - exp(-nVal)));
}

void HaldwaniMapWidget::fetchTile(int z, int x, int y, bool isRetry) {
    QString key = QString("%1_%2_%3").arg(z).arg(x).arg(y);
    if (m_tileCache.contains(key) || m_pendingRequests.contains(key)) return;

    m_pendingRequests.insert(key);

    QString urlStr;
    if (!isRetry) {
        urlStr = QString("https://tile.openstreetmap.org/%1/%2/%3.png").arg(z).arg(x).arg(y);
    } else {
        urlStr = QString("http://a.tile.openstreetmap.org/%1/%2/%3.png").arg(z).arg(x).arg(y);
    }

    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "AlgoRoute/1.0");

    QNetworkReply *reply = m_netManager->get(req);
    reply->setProperty("tileKey", key);
    reply->setProperty("zoom", z);
    reply->setProperty("tileX", x);
    reply->setProperty("tileY", y);
    reply->setProperty("isRetry", isRetry);
}

void HaldwaniMapWidget::onTileDownloaded(QNetworkReply *reply) {
    QString key = reply->property("tileKey").toString();
    bool isRetry = reply->property("isRetry").toBool();
    int z = reply->property("zoom").toInt();
    int x = reply->property("tileX").toInt();
    int y = reply->property("tileY").toInt();

    m_pendingRequests.remove(key);

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap *pm = new QPixmap();
        if (pm->loadFromData(data)) {
            m_tileCache.insert(key, pm);
            update();
        } else {
            delete pm;
        }
    } else if (!isRetry) {
        fetchTile(z, x, y, true);
    }

    reply->deleteLater();
}

void HaldwaniMapWidget::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    m_zoomInRect = QRect(15, 15, 36, 36);
    m_zoomOutRect = QRect(15, 55, 36, 36);
}

void HaldwaniMapWidget::drawMapPin(QPainter &painter, const QPointF &pt, const QColor &color, const QString &label, bool isStart) {
    double x = pt.x();
    double y = pt.y();

    // 1. Draw Ground Pulse Aura
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(color.red(), color.green(), color.blue(), 50));
    painter.drawEllipse(QPointF(x, y), 16, 8);

    // 2. Draw Classic Map Pin Teardrop Path
    QPainterPath pinPath;
    double headR = 12.0;
    double headCY = y - 24.0;

    pinPath.addEllipse(QPointF(x, headCY), headR, headR);

    QPolygonF tipTriangle;
    tipTriangle << QPointF(x - 8, headCY + 6)
                << QPointF(x + 8, headCY + 6)
                << QPointF(x, y);
    pinPath.addPolygon(tipTriangle);

    painter.setBrush(color);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawPath(pinPath);

    // Inner Pin White Dot
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(x, headCY), 4.5, 4.5);

    // 3. Draw Label Card Box
    QString text = (isStart ? "🟢 " : "🔴 ") + label;
    QFont font("Outfit", 10, QFont::Bold);
    painter.setFont(font);
    QFontMetrics fm(font);
    int tw = fm.horizontalAdvance(text) + 16;

    QRect labelRect(x + 16, headCY - 12, tw, 26);
    painter.setBrush(QColor(15, 23, 42, 230));
    painter.setPen(QPen(color, 1.5));
    painter.drawRoundedRect(labelRect, 6, 6);

    painter.setPen(QColor("#f8fafc"));
    painter.drawText(labelRect, Qt::AlignCenter, text);
}

void HaldwaniMapWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dark Map Background
    painter.fillRect(rect(), QColor("#090d16"));

    // 1. Calculate & Draw OpenStreetMap Raster Map Tiles
    int n = 1 << m_zoom;
    double centerWorldX = (m_centerLon + 180.0) / 360.0 * n * 256.0;
    double centerLatRad = m_centerLat * M_PI / 180.0;
    double centerWorldY = (1.0 - log(tan(centerLatRad) + 1.0 / cos(centerLatRad)) / M_PI) / 2.0 * n * 256.0;

    double topLeftWorldX = centerWorldX - width() / 2.0;
    double topLeftWorldY = centerWorldY - height() / 2.0;

    int minTileX = static_cast<int>(floor(topLeftWorldX / 256.0));
    int maxTileX = static_cast<int>(floor((topLeftWorldX + width()) / 256.0));
    int minTileY = static_cast<int>(floor(topLeftWorldY / 256.0));
    int maxTileY = static_cast<int>(floor((topLeftWorldY + height()) / 256.0));

    for (int tx = minTileX; tx <= maxTileX; ++tx) {
        for (int ty = minTileY; ty <= maxTileY; ++ty) {
            double screenTileX = width() / 2.0 + (tx * 256.0 - centerWorldX);
            double screenTileY = height() / 2.0 + (ty * 256.0 - centerWorldY);

            QString key = QString("%1_%2_%3").arg(m_zoom).arg(tx).arg(ty);
            QPixmap *pm = m_tileCache.object(key);

            if (pm && !pm->isNull()) {
                painter.drawPixmap(static_cast<int>(screenTileX), static_cast<int>(screenTileY), 256, 256, *pm);
            } else {
                fetchTile(m_zoom, tx, ty);
                painter.setPen(QPen(QColor(255, 255, 255, 20)));
                painter.drawRect(static_cast<int>(screenTileX), static_cast<int>(screenTileY), 256, 256);
            }
        }
    }

    // 2. Draw Explored Nodes (Glowing Cyan Dots during Search)
    if (!m_exploredNodes.isEmpty() && m_graph) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(6, 182, 212, 180));

        for (long long nid : m_exploredNodes) {
            auto it = m_graph->nodes.find(nid);
            if (it != m_graph->nodes.end()) {
                QPointF pt = latLonToScreen(it.value().lat, it.value().lon);
                painter.drawEllipse(pt, 3.5, 3.5);
            }
        }
    }

    // 3. Draw Optimal Shortest Path Polyline (Thick Vibrant Cyan Polyline)
    if (m_pathNodes.size() >= 2 && m_graph) {
        QPen pathPen(QColor("#06b6d4"));
        pathPen.setWidth(6);
        pathPen.setCapStyle(Qt::RoundCap);
        pathPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pathPen);

        QPainterPath path;

        // Path start point
        QPointF firstPt;
        if (m_hasStart) {
            firstPt = latLonToScreen(m_startLat, m_startLon);
        } else {
            auto startIt = m_graph->nodes.find(m_pathNodes[0]);
            if (startIt != m_graph->nodes.end()) firstPt = latLonToScreen(startIt.value().lat, startIt.value().lon);
        }
        path.moveTo(firstPt);

        for (int i = 0; i < m_pathNodes.size(); ++i) {
            auto it = m_graph->nodes.find(m_pathNodes[i]);
            if (it != m_graph->nodes.end()) {
                path.lineTo(latLonToScreen(it.value().lat, it.value().lon));
            }
        }

        if (m_hasDest) {
            path.lineTo(latLonToScreen(m_destLat, m_destLon));
        }

        painter.drawPath(path);
    }

    // 4. Draw Start Marker (Emerald Green Pin)
    if (m_hasStart) {
        QPointF p = latLonToScreen(m_startLat, m_startLon);
        drawMapPin(painter, p, QColor("#10b981"), m_startName, true);
    }

    // 5. Draw Destination Marker (Rose Red Pin)
    if (m_hasDest) {
        QPointF p = latLonToScreen(m_destLat, m_destLon);
        drawMapPin(painter, p, QColor("#ef4444"), m_destName, false);
    }

    // 6. Draw Leaflet-style Zoom Buttons (+ / -) in Top Left
    m_zoomInRect = QRect(15, 15, 34, 34);
    m_zoomOutRect = QRect(15, 54, 34, 34);

    painter.setBrush(QColor(15, 23, 42, 230));
    painter.setPen(QPen(QColor(255, 255, 255, 40), 1));
    painter.drawRoundedRect(m_zoomInRect, 6, 6);
    painter.drawRoundedRect(m_zoomOutRect, 6, 6);

    painter.setFont(QFont("Outfit", 16, QFont::Bold));
    painter.setPen(QColor("#f8fafc"));
    painter.drawText(m_zoomInRect, Qt::AlignCenter, "+");
    painter.drawText(m_zoomOutRect, Qt::AlignCenter, "-");

    // 7. Draw Leaflet Legend Box in Bottom Right
    int legW = 160;
    int legH = 120;
    QRect legendRect(width() - legW - 15, height() - legH - 15, legW, legH);
    painter.setBrush(QColor(15, 23, 42, 235));
    painter.setPen(QPen(QColor(255, 255, 255, 30), 1));
    painter.drawRoundedRect(legendRect, 10, 10);

    painter.setFont(QFont("Outfit", 9, QFont::Bold));
    int ly = legendRect.top() + 18;
    
    painter.setBrush(QColor("#10b981"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(legendRect.left() + 15, ly - 5, 10, 10);
    painter.setPen(QColor("#f8fafc"));
    painter.drawText(legendRect.left() + 35, ly + 3, "Start Location");

    ly += 24;
    painter.setBrush(QColor("#ef4444"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(legendRect.left() + 15, ly - 5, 10, 10);
    painter.setPen(QColor("#f8fafc"));
    painter.drawText(legendRect.left() + 35, ly + 3, "Destination");

    ly += 24;
    painter.setBrush(QColor("#06b6d4"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(legendRect.left() + 15, ly - 5, 10, 10);
    painter.setPen(QColor("#f8fafc"));
    painter.drawText(legendRect.left() + 35, ly + 3, "Explored Node");

    ly += 24;
    painter.setPen(QPen(QColor("#06b6d4"), 3));
    painter.drawLine(legendRect.left() + 12, ly - 1, legendRect.left() + 27, ly - 1);
    painter.setPen(QColor("#f8fafc"));
    painter.drawText(legendRect.left() + 35, ly + 3, "Optimal Route");
}

void HaldwaniMapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_zoomInRect.contains(event->pos())) {
            m_zoom = min(m_zoom + 1, 18);
            update();
            return;
        }
        if (m_zoomOutRect.contains(event->pos())) {
            m_zoom = max(m_zoom - 1, 11);
            update();
            return;
        }

        m_isDragging = true;
        m_pressPos = event->pos();
        m_lastMousePos = event->pos();
    }
}

void HaldwaniMapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        int n = 1 << m_zoom;
        double centerWorldX = (m_centerLon + 180.0) / 360.0 * n * 256.0;
        double centerLatRad = m_centerLat * M_PI / 180.0;
        double centerWorldY = (1.0 - log(tan(centerLatRad) + 1.0 / cos(centerLatRad)) / M_PI) / 2.0 * n * 256.0;

        centerWorldX -= delta.x();
        centerWorldY -= delta.y();

        m_centerLon = (centerWorldX / 256.0 / n) * 360.0 - 180.0;
        double nVal = M_PI - 2.0 * M_PI * (centerWorldY / 256.0 / n);
        m_centerLat = 180.0 / M_PI * atan(0.5 * (exp(nVal) - exp(-nVal)));

        update();
    }
}

void HaldwaniMapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;

        if ((event->pos() - m_pressPos).manhattanLength() < 5 && m_graph) {
            double lat = 0, lon = 0;
            screenToLatLon(event->pos(), lat, lon);

            long long nearestId = m_graph->findNearestNode(lat, lon);
            if (nearestId != -1 && m_graph->nodes.contains(nearestId)) {
                const Node &node = m_graph->nodes.value(nearestId);
                m_clickedNodeId = nearestId;
                m_clickedLat = lat;
                m_clickedLon = lon;

                if (!node.name.isEmpty() && node.name != "null") {
                    m_clickedNodeName = node.name;
                } else {
                    m_clickedNodeName = QString("Location (%1, %2)").arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4);
                }

                m_clickMenu->exec(event->globalPosition().toPoint());
            }
        }
    }
}

void HaldwaniMapWidget::onSetStartTriggered() {
    if (m_clickedNodeId != -1) {
        setStartLocation(m_clickedNodeId, m_clickedLat, m_clickedLon, m_clickedNodeName);
        emit startLocationSelected(m_clickedNodeId, m_clickedNodeName);
    }
}

void HaldwaniMapWidget::onSetDestTriggered() {
    if (m_clickedNodeId != -1) {
        setDestLocation(m_clickedNodeId, m_clickedLat, m_clickedLon, m_clickedNodeName);
        emit destLocationSelected(m_clickedNodeId, m_clickedNodeName);
    }
}

void HaldwaniMapWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        m_zoom = min(m_zoom + 1, 18);
    } else {
        m_zoom = max(m_zoom - 1, 11);
    }
    update();
}
