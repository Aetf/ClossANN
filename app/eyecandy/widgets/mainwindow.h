#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVariantList>
#include <memory>
#include "models/learnparam.h"
#include "models/ucwdataset.h"

namespace Ui {
class MainWindow;
}
class UIHandler;
class LearnTask;
class LayerDescModel;
class QCustomPlot;
class QCPAxis;
class QCPColorMap;
class QCPColorScale;
class QCPGraph;
class QCPItemTracer;
class ColumnResizer;

using std::unique_ptr;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupProblemPlane(QCustomPlot *plot);
    void setupErrorLine(QCustomPlot *plot);

protected:
    void setupToolbar();
    void setupOptionPage();
    void setupMonitorPage();
    void setupLogPage();

    void displayDefaultOptions();
    void applyOptions();

    void trainClossNN();
    void stopTraining();

    QCPColorScale *createPredictColorScale(QCustomPlot *plot);
    QCPColorMap *createPredictMap(QCPAxis *xAxis, QCPAxis *yAxis, QCPColorScale *scale);
    void setupTrainingGraph(QCustomPlot *plot, range outputRange, int labelsCount);
    void setupTestingGraph(QCustomPlot *plot, range outputRange, int labelsCount);

private slots:
    void screenShot();
    void updateButtons(const QModelIndex &curr, const QModelIndex &prev);

private:
    unique_ptr<Ui::MainWindow> ui;

    unique_ptr<UIHandler> handler;

    LayerDescModel *layersModel;
    QCPColorMap *predictMap;
    QCPGraph *trainingGraph;
    QCPGraph *testingGraph;
    double lastIterTime;
    QString demoName;
    QTimer predictionTimer;
    QTimer dataTimer2;
    QCustomPlot *bracketPlot;
    QCPItemTracer *itemDemoPhaseTracer;

    LearnParam currentParam;
    int currentDemoIndex;
};

#endif // MAINWINDOW_H
