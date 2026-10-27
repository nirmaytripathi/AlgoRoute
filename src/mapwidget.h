#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QCache>
#include <QSet>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QList>
#include "graph.h"

using namespace std;

class HaldwaniMapWidget : public QWidget {
    Q_OBJECT

public:
    explicit HaldwaniMapWidget(QWidget *parent = nullptr);
    ~HaldwaniMapWidget();

    void setGraph(const Graph *graph);
    void setStartLocation(long long nodeId, double lat, double lon, const QString &name);
    void setDestLocation(long long nodeId, double lat, double lon, const QString &name);
    void setStartNode(long long nodeId, const QString &name);
    void setDestNode(long long nodeId, const QString &name);
    void setOptimalPath(const QList<long long> &pathNodes);
    void startExplorationAnimation(const QList<long long> &exploredNodes, const QList<long long> &finalPath, int speedMultiplier);
    void clearMap();
    void autoFitView();

    long long startNodeId() const { return m_startNodeId; }
    long long destNodeId() const { return m_destNodeId; }
    QString startName() const { return m_startName; }
    QString destName() const { return m_destName; }

signals:
    void startLocationSelected(long long nodeId, const QString &name);
    void destLocationSelected(long long nodeId, const QString &name);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTileDownloaded(QNetworkReply *reply);
    void onSetStartTriggered();
    void onSetDestTriggered();
    void onAnimStep();

private:
    void fetchTile(int z, int x, int y, bool isRetry = false);
    void drawMapPin(QPainter &painter, const QPointF &pt, const QColor &color, const QString &label, bool isStart);

    QPointF latLonToScreen(double lat, double lon) const;
    void screenToLatLon(const QPointF &screenPos, double &lat, double &lon) const;

    static int lonToTileX(double lon, int z);
    static int latToTileY(double lat, int z);
    static double tileXToLon(double x, int z);
    static double tileYToLat(double y, int z);

    const Graph *m_graph = nullptr;

    bool m_hasStart = false;
    bool m_hasDest = false;
    long long m_startNodeId = -1;
    long long m_destNodeId = -1;
    double m_startLat = 0.0;
    double m_startLon = 0.0;
    double m_destLat = 0.0;
    double m_destLon = 0.0;
    QString m_startName;
    QString m_destName;

    QList<long long> m_pathNodes;
    QList<long long> m_exploredNodes;

    // Smooth Timer Animation State
    QTimer *m_animTimer;
    QList<long long> m_fullExploredList;
    QList<long long> m_finalPathNodes;
    int m_animIndex = 0;
    int m_animBatchSize = 1;

    // Viewport Center
    double m_centerLat = 29.2188;
    double m_centerLon = 79.5130;
    int m_zoom = 14;

    bool m_isDragging = false;
    QPoint m_pressPos;
    QPoint m_lastMousePos;

    // Tile Network Manager & Memory Cache
    QNetworkAccessManager *m_netManager;
    QCache<QString, QPixmap> m_tileCache;
    QSet<QString> m_pendingRequests;

    // Click Context Menu
    QMenu *m_clickMenu;
    QAction *m_setStartAction;
    QAction *m_setDestAction;
    long long m_clickedNodeId = -1;
    double m_clickedLat = 0.0;
    double m_clickedLon = 0.0;
    QString m_clickedNodeName;

    // UI Buttons rects for +/- zoom
    QRect m_zoomInRect;
    QRect m_zoomOutRect;
};

#endif // MAPWIDGET_H
