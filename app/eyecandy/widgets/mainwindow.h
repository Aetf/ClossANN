#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVariantList>
#include <memory>

namespace Ui {
class MainWindow;
}
class UIHandler;
class QCustomPlot;
class QCPAxis;
class QCPColorMap;
class QCPColorScale;
class QCPGraph;
class QCPItemTracer;
class QtAwesome;

using std::unique_ptr;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupDemo(int demoIndex, QCustomPlot *plot);
    void setupScatterStyleDemo(QCustomPlot *customPlot);
    void setupLineStyleDemo(QCustomPlot *customPlot);
    void setupParametricCurveDemo(QCustomPlot *customPlot);
    void setupBarChartDemo(QCustomPlot *customPlot);
    void setupItemDemo(QCustomPlot *customPlot);
    void setupStyledDemo(QCustomPlot *customPlot);
    void setupAdvancedAxesDemo(QCustomPlot *customPlot);

    void setupPlayground();
    void setupProblemPlane(QCustomPlot *plot);
    void setupErrorLine(QCustomPlot *plot);

protected:
    void setupToolbar();
    QCPColorScale *createPredictColorScale(QCustomPlot *plot);
    QCPColorMap *createPredictMap(QCPAxis *xAxis, QCPAxis *yAxis, QCPColorScale *scale);
    void setupTrainingGraph(QCustomPlot *plot);
    void setupTestingGraph(QCustomPlot *plot);

private slots:
    void trainClossNN();
    void bracketDataSlot();
    void screenShot();

    void onTrainIterationFinished(int iter, double error);

private:
    unique_ptr<Ui::MainWindow> ui;
    unique_ptr<UIHandler> handler;
    unique_ptr<QtAwesome> awesome;

    QCPColorMap *predictMap;
    QCPGraph *trainingGraph;
    QCPGraph *testingGraph;
    QString demoName;
    QTimer dataTimer;
    QTimer dataTimer2;
    QCustomPlot *bracketPlot;
    QCPItemTracer *itemDemoPhaseTracer;
    int currentDemoIndex;
};

#endif // MAINWINDOW_H
