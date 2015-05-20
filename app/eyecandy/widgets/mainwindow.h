#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVariantList>
#include <qcustomplot.h>
#include <memory>

namespace Ui {
class MainWindow;
}
class UIHandler;
class QCustomPlot;
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
    void setupErrorLine(QCustomPlot *customPlot);

protected:
    void setupToolbar();

private slots:
    void trainClossNN();
    void bracketDataSlot();
    void screenShot();

    void onPredictionUpdated(QVariantList data);
    void onTestingDataUpdated(QVariantList data);
    void onTrainingDataUpdated(QVariantList data);
    void onTrainIterationFinished(int iter, double error);

private:
    unique_ptr<Ui::MainWindow> ui;
    unique_ptr<UIHandler> handler;
    QString demoName;
    QTimer dataTimer;
    QTimer dataTimer2;
    QCustomPlot *bracketPlot;
    QCPItemTracer *itemDemoPhaseTracer;
    unique_ptr<QtAwesome> awesome;
    int currentDemoIndex;
};

#endif // MAINWINDOW_H
