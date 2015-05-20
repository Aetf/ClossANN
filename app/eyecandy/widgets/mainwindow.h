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
    void setupQuadraticDemo(QCustomPlot *customPlot);
    void setupSimpleDemo(QCustomPlot *customPlot);
    void setupSincScatterDemo(QCustomPlot *customPlot);
    void setupScatterStyleDemo(QCustomPlot *customPlot);
    void setupLineStyleDemo(QCustomPlot *customPlot);
    void setupScatterPixmapDemo(QCustomPlot *customPlot);
    void setupDateDemo(QCustomPlot *customPlot);
    void setupTextureBrushDemo(QCustomPlot *customPlot);
    void setupMultiAxisDemo(QCustomPlot *customPlot);
    void setupLogarithmicDemo(QCustomPlot *customPlot);
    void setupParametricCurveDemo(QCustomPlot *customPlot);
    void setupBarChartDemo(QCustomPlot *customPlot);
    void setupStatisticalDemo(QCustomPlot *customPlot);
    void setupSimpleItemDemo(QCustomPlot *customPlot);
    void setupItemDemo(QCustomPlot *customPlot);
    void setupStyledDemo(QCustomPlot *customPlot);
    void setupAdvancedAxesDemo(QCustomPlot *customPlot);
    void setupColorMapDemo(QCustomPlot *customPlot);
    void setupFinancialDemo(QCustomPlot *customPlot);

    void setupPlayground();
    void setupColorMap(QCustomPlot *customPlot);
    void setupErrorLine(QCustomPlot *customPlot);

protected:
    void setupToolbar();

private slots:
    void clossnnDataSlot();
    void realtimeDataSlot();
    void bracketDataSlot();
    void screenShot();

    void onPredictionUpdated(QVariantList data);
    void onTestingDataUpdated(QVariantList data);
    void onTrainingDataUpdated(QVariantList data);
    void onIterationFinished(int iter, double error);

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
