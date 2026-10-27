#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dijkstra.h"
#include "astar.h"
#include <QHeaderView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <algorithm>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup Algo ComboBox
    ui->algoCombo->addItem("A* Search Algorithm (Heuristic Directed)", "astar");
    ui->algoCombo->addItem("Dijkstra's Algorithm (Uniform Cost)", "dijkstra");

    // Table Header Setup & Stretching
    ui->benchmarkTable->setColumnCount(5);
    ui->benchmarkTable->setHorizontalHeaderLabels({"Algorithm", "Path Distance (km)", "Nodes Explored", "Execution Time (ms)", "Efficiency Gain"});
    ui->benchmarkTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Load Graph & Presets
    if (m_graph.loadFromJSON(":/data/haldwani_graph.json")) {
        ui->mapWidget->setGraph(&m_graph);
    }
    loadPresets();

    // Signal / Slot Connections
    connect(ui->startCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStartPresetChanged);
    connect(ui->destCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDestPresetChanged);
    connect(ui->swapBtn, &QPushButton::clicked, this, &MainWindow::onSwapLocationsClicked);
    connect(ui->findRouteBtn, &QPushButton::clicked, this, &MainWindow::onFindRouteClicked);
    connect(ui->visualizeBtn, &QPushButton::clicked, this, &MainWindow::onVisualizeClicked);
    connect(ui->compareBtn, &QPushButton::clicked, this, &MainWindow::onCompareClicked);
    connect(ui->clearBtn, &QPushButton::clicked, this, &MainWindow::onClearClicked);

    connect(ui->speedSlider, &QSlider::valueChanged, [this](int val) {
        m_animSpeed = val;
        ui->speedLabel->setText(QString("%1x").arg(val));
    });

    connect(ui->mapWidget, &HaldwaniMapWidget::startLocationSelected, this, &MainWindow::onStartLocationSelectedOnMap);
    connect(ui->mapWidget, &HaldwaniMapWidget::destLocationSelected, this, &MainWindow::onDestLocationSelectedOnMap);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadPresets() {
    QFile file(":/data/preset_locations.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    const QJsonArray arr = doc.array();
    m_presets.clear();

    ui->startCombo->addItem("-- Select Starting Landmark --");
    ui->destCombo->addItem("-- Select Destination Landmark --");

    for (const QJsonValue &val : std::as_const(arr)) {
        QJsonObject obj = val.toObject();
        LandmarkPreset p;
        p.name = obj["name"].toString();
        p.lat = obj["lat"].toDouble();
        p.lon = obj["lon"].toDouble();

        long long nId = -1;
        if (obj.contains("nodeId") && !obj["nodeId"].isNull()) {
            nId = obj["nodeId"].toVariant().toLongLong();
        }
        if (nId <= 0 || !m_graph.nodes.contains(nId)) {
            nId = m_graph.findNearestNode(p.lat, p.lon);
        }
        p.nodeId = nId;
        m_presets.append(p);

        ui->startCombo->addItem("📍 " + p.name);
        ui->destCombo->addItem("🎯 " + p.name);
    }
}

void MainWindow::onStartPresetChanged(int index) {
    if (index > 0 && index - 1 < m_presets.size()) {
        LandmarkPreset p = m_presets[index - 1];
        m_startNodeId = p.nodeId;
        ui->startEdit->setText("🟢 " + p.name);
        ui->mapWidget->setStartLocation(p.nodeId, p.lat, p.lon, p.name);
    }
}

void MainWindow::onDestPresetChanged(int index) {
    if (index > 0 && index - 1 < m_presets.size()) {
        LandmarkPreset p = m_presets[index - 1];
        m_destNodeId = p.nodeId;
        ui->destEdit->setText("🔴 " + p.name);
        ui->mapWidget->setDestLocation(p.nodeId, p.lat, p.lon, p.name);
    }
}

void MainWindow::onSwapLocationsClicked() {
    std::swap(m_startNodeId, m_destNodeId);

    QString startText = ui->startEdit->text();
    QString destText = ui->destEdit->text();
    ui->startEdit->setText(destText);
    ui->destEdit->setText(startText);

    ui->mapWidget->setStartNode(m_startNodeId, ui->startEdit->text());
    ui->mapWidget->setDestNode(m_destNodeId, ui->destEdit->text());
}

void MainWindow::onStartLocationSelectedOnMap(long long nodeId, const QString &name) {
    m_startNodeId = nodeId;
    ui->startEdit->setText("🟢 " + name);
}

void MainWindow::onDestLocationSelectedOnMap(long long nodeId, const QString &name) {
    m_destNodeId = nodeId;
    ui->destEdit->setText("🔴 " + name);
}

void MainWindow::onFindRouteClicked() {
    if (m_startNodeId == -1 || m_destNodeId == -1) return;

    QString algo = ui->algoCombo->currentData().toString();

    if (algo == "dijkstra") {
        DijkstraResult res = runDijkstra(m_graph, m_startNodeId, m_destNodeId);
        if (res.success) {
            displayResultCard(res.totalDistanceMeters, res.nodesExploredCount, res.executionTimeMs, "Dijkstra");
            ui->mapWidget->setOptimalPath(res.pathNodes);
        }
    } else {
        AStarResult res = runAStar(m_graph, m_startNodeId, m_destNodeId);
        if (res.success) {
            displayResultCard(res.totalDistanceMeters, res.nodesExploredCount, res.executionTimeMs, "A*");
            ui->mapWidget->setOptimalPath(res.pathNodes);
        }
    }
}

void MainWindow::displayResultCard(double distMeters, int nodesExplored, double timeMs, const QString& algoName) {
    Q_UNUSED(distMeters);
    Q_UNUSED(nodesExplored);
    Q_UNUSED(timeMs);
    Q_UNUSED(algoName);
}

void MainWindow::onVisualizeClicked() {
    if (m_startNodeId == -1 || m_destNodeId == -1) return;

    QString algo = ui->algoCombo->currentData().toString();

    if (algo == "dijkstra") {
        DijkstraResult res = runDijkstra(m_graph, m_startNodeId, m_destNodeId);
        if (res.success) {
            displayResultCard(res.totalDistanceMeters, res.nodesExploredCount, res.executionTimeMs, "Dijkstra (Visualizing)");
            ui->mapWidget->startExplorationAnimation(res.exploredNodesList, res.pathNodes, m_animSpeed);
        }
    } else {
        AStarResult res = runAStar(m_graph, m_startNodeId, m_destNodeId);
        if (res.success) {
            displayResultCard(res.totalDistanceMeters, res.nodesExploredCount, res.executionTimeMs, "A* (Visualizing)");
            ui->mapWidget->startExplorationAnimation(res.exploredNodesList, res.pathNodes, m_animSpeed);
        }
    }
}

void MainWindow::onCompareClicked() {
    if (m_startNodeId == -1 || m_destNodeId == -1) return;

    DijkstraResult dijRes = runDijkstra(m_graph, m_startNodeId, m_destNodeId);
    AStarResult astarRes = runAStar(m_graph, m_startNodeId, m_destNodeId);

    ui->benchmarkTable->setColumnCount(5);
    ui->benchmarkTable->setHorizontalHeaderLabels({"Algorithm", "Path Distance (km)", "Nodes Explored", "Execution Time (ms)", "Efficiency Gain"});
    ui->benchmarkTable->setRowCount(2);

    auto makeItem = [](const QString &str, bool center = false) {
        QTableWidgetItem *item = new QTableWidgetItem(str);
        item->setForeground(QColor("#f8fafc"));
        if (center) item->setTextAlignment(Qt::AlignCenter);
        return item;
    };

    // Row 0: Dijkstra
    ui->benchmarkTable->setItem(0, 0, makeItem("Dijkstra's Algorithm"));
    ui->benchmarkTable->setItem(0, 1, makeItem(QString::number(dijRes.totalDistanceMeters / 1000.0, 'f', 2) + " km", true));
    ui->benchmarkTable->setItem(0, 2, makeItem(QString::number(dijRes.nodesExploredCount), true));
    ui->benchmarkTable->setItem(0, 3, makeItem(QString::number(dijRes.executionTimeMs, 'f', 2) + " ms", true));
    ui->benchmarkTable->setItem(0, 4, makeItem("Baseline (100% Search Area)", true));

    // Row 1: A*
    ui->benchmarkTable->setItem(1, 0, makeItem("A* Search Algorithm"));
    ui->benchmarkTable->setItem(1, 1, makeItem(QString::number(astarRes.totalDistanceMeters / 1000.0, 'f', 2) + " km", true));
    ui->benchmarkTable->setItem(1, 2, makeItem(QString::number(astarRes.nodesExploredCount), true));
    ui->benchmarkTable->setItem(1, 3, makeItem(QString::number(astarRes.executionTimeMs, 'f', 2) + " ms", true));

    double efficiency = 0.0;
    if (dijRes.nodesExploredCount > 0) {
        efficiency = ((double)(dijRes.nodesExploredCount - astarRes.nodesExploredCount) / dijRes.nodesExploredCount) * 100.0;
    }
    ui->benchmarkTable->setItem(1, 4, makeItem(QString("%1% Fewer Nodes Explored 🚀").arg(efficiency, 0, 'f', 1), true));

    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::onClearClicked() {
    m_startNodeId = -1;
    m_destNodeId = -1;
    ui->startEdit->clear();
    ui->destEdit->clear();
    ui->startCombo->setCurrentIndex(0);
    ui->destCombo->setCurrentIndex(0);
    ui->benchmarkTable->setRowCount(0);
    ui->mapWidget->clearMap();
}
