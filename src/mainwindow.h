#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include "graph.h"

using namespace std;

namespace Ui {
class MainWindow;
}

struct LandmarkPreset {
    QString name;
    long long nodeId;
    double lat;
    double lon;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartPresetChanged(int index);
    void onDestPresetChanged(int index);
    void onSwapLocationsClicked();
    void onFindRouteClicked();
    void onVisualizeClicked();
    void onCompareClicked();
    void onClearClicked();
    void onStartLocationSelectedOnMap(long long nodeId, const QString &name);
    void onDestLocationSelectedOnMap(long long nodeId, const QString &name);

private:
    void loadPresets();
    void displayResultCard(double distMeters, int nodesExplored, double timeMs, const QString& algoName);

    Ui::MainWindow *ui;

    Graph m_graph;
    QList<LandmarkPreset> m_presets;

    long long m_startNodeId = -1;
    long long m_destNodeId = -1;

    int m_animSpeed = 5;
};

#endif // MAINWINDOW_H
