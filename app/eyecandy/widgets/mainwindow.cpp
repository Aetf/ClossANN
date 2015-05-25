#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QAction>
#include <QDesktopWidget>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTabWidget>
#include <QScreen>
#include <QList>
#include <QMessageBox>
#include <QMetaEnum>
#include <qcustomplot.h>
#include "logic/uihandler.h"
#include "models/layerdescmodel.h"
#include "widgets/layerdelegate.h"
#include "utils.h"
#include "QtAwesome.h"
#include "columnresizer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , handler(new UIHandler)
    , awesome(new QtAwesome)
    , predictMap(nullptr)
    , trainingGraph(nullptr)
    , testingGraph(nullptr)
{
    awesome->initFontAwesome();
    ui->setupUi(this);
    setupToolbar();
    setupOptionPage();

    setupPlayground();

    ui->tabs->setUpdatesEnabled(false);
    for (int i = 1; i!= 8; i++) {
        auto plot = new QCustomPlot;
        setupDemo(i, plot);
        ui->tabs->addTab(plot, demoName);
    }
    ui->tabs->setUpdatesEnabled(true);
}

void MainWindow::setupToolbar()
{
    ui->actionTrain->setIcon(awesome->icon(fa::play));
    connect(ui->actionTrain, &QAction::triggered,
            this, &MainWindow::trainClossNN);
    ui->toolBar->addAction(ui->actionTrain);
}

void MainWindow::setupOptionPage()
{
    auto colResizer = new ColumnResizer(this);
    colResizer->addWidgetsFromLayout(ui->groupNet->layout(), 0);
    colResizer->addWidgetsFromLayout(ui->groupCloss->layout(), 0);
    
    connect(ui->spinLearnRate, Select<double>::OverloadOf(&QDoubleSpinBox::valueChanged),
            this, [=](auto value){
        this->currentParam.learningRate(value);
    });
    connect(ui->spinKernelSize, Select<double>::OverloadOf(&QDoubleSpinBox::valueChanged),
            this, [=](auto value){
        this->currentParam.kernelSize(value);
    });
    connect(ui->spinPValue, Select<double>::OverloadOf(&QDoubleSpinBox::valueChanged),
            this, [=](auto value){
        this->currentParam.pValue(value);
    });

    auto layersModel = new LayerDescModel(this);
    ui->tableNetStru->setModel(layersModel);
    ui->tableNetStru->setItemDelegate(new LayerDelegate(this));
    ui->tableNetStru->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableNetStru->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->tableNetStru->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::updateButtons);

    connect(ui->btnInsertLayer, &QPushButton::clicked,
            this, [=]{
        auto pos = this->ui->tableNetStru->selectionModel()->currentIndex().row();
        layersModel->insertLayer(pos + 1, LayerDesc());
    });
    connect(ui->btnRemoveLayer, &QPushButton::clicked,
            this, [=]{
        auto pos = this->ui->tableNetStru->selectionModel()->currentIndex().row();
        layersModel->removeRows(pos, 1);
    });
    connect(ui->btnApplyConfig, &QPushButton::clicked,
            this, [=]{
        LearnParam param(ui->spinLearnRate->value(),
                         ui->spinKernelSize->value(),
                         ui->spinPValue->value());
        param.layers(layersModel->layers());

        handler->configure(param);
    });

    displayDefaultOptions();
    ui->tableNetStru->selectRow(0);
}

void MainWindow::displayDefaultOptions()
{
    LearnParam param;
    ui->spinKernelSize->setValue(param.kernelSize());
    ui->spinLearnRate->setValue(param.learningRate());
    ui->spinPValue->setValue(param.pValue());
    auto model = ui->tableNetStru->model();
    model->removeRows(0, model->rowCount());
    model->insertRows(0, param.layers().size());
    for (int i = 0; i!= param.layers().size(); i++) {
        QVariant v;
        v.setValue(param.layers()[i]);
        model->setData(model->index(i, 0), v, LayerDescModel::LayerDataRole);
    }
}

void MainWindow::updateButtons(const QModelIndex &curr, const QModelIndex &prev)
{
    auto row = curr.row();
    if (row == 0) {
        ui->btnRemoveLayer->setEnabled(false);
        ui->btnInsertLayer->setEnabled(true);
    } else if (row == ui->tableNetStru->model()->rowCount() - 1) {
        ui->btnRemoveLayer->setEnabled(false);
        ui->btnInsertLayer->setEnabled(false);
    } else {
        ui->btnRemoveLayer->setEnabled(true);
        ui->btnInsertLayer->setEnabled(true);
    }
}

void MainWindow::setupDemo(int demoIndex, QCustomPlot *plot)
{
    switch (demoIndex)
    {
    case 1:
        setupScatterStyleDemo(plot);
        break;
    case 2:
        setupLineStyleDemo(plot);
        break;
    case 3:
        setupParametricCurveDemo(plot);
        break;
    case 4:
        setupBarChartDemo(plot);
        break;
    case 5:
        setupItemDemo(plot);
        break;
    case 6:
        setupStyledDemo(plot);
        break;
    case 7:
        setupAdvancedAxesDemo(plot);
        break;
    }
    setWindowTitle("QCustomPlot: "+demoName);
    statusBar()->clearMessage();
    currentDemoIndex = demoIndex;
    plot->replot();
}

void MainWindow::setupScatterStyleDemo(QCustomPlot *customPlot)
{
    demoName = "Scatter Style Demo";
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setRowSpacing(-3);
    QVector<QCPScatterStyle::ScatterShape> shapes;
    shapes << QCPScatterStyle::ssCross;
    shapes << QCPScatterStyle::ssPlus;
    shapes << QCPScatterStyle::ssCircle;
    shapes << QCPScatterStyle::ssDisc;
    shapes << QCPScatterStyle::ssSquare;
    shapes << QCPScatterStyle::ssDiamond;
    shapes << QCPScatterStyle::ssStar;
    shapes << QCPScatterStyle::ssTriangle;
    shapes << QCPScatterStyle::ssTriangleInverted;
    shapes << QCPScatterStyle::ssCrossSquare;
    shapes << QCPScatterStyle::ssPlusSquare;
    shapes << QCPScatterStyle::ssCrossCircle;
    shapes << QCPScatterStyle::ssPlusCircle;
    shapes << QCPScatterStyle::ssPeace;
    shapes << QCPScatterStyle::ssCustom;

    QPen pen;
    // add graphs with different scatter styles:
    for (int i=0; i<shapes.size(); ++i)
    {
        customPlot->addGraph();
        pen.setColor(QColor(qSin(i*0.3)*100+100, qSin(i*0.6+0.7)*100+100, qSin(i*0.4+0.6)*100+100));
        // generate data:
        QVector<double> x(10), y(10);
        for (int k=0; k<10; ++k)
        {
            x[k] = k/10.0 * 4*3.14 + 0.01;
            y[k] = 7*qSin(x[k])/x[k] + (shapes.size()-i)*5;
        }
        customPlot->graph()->setData(x, y);
        customPlot->graph()->rescaleAxes(true);
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setName(QCPScatterStyle::staticMetaObject.enumerator(QCPScatterStyle::staticMetaObject.indexOfEnumerator("ScatterShape")).valueToKey(shapes.at(i)));
        customPlot->graph()->setLineStyle(QCPGraph::lsLine);
        // set scatter style:
        if (shapes.at(i) != QCPScatterStyle::ssCustom)
        {
            customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes.at(i), 10));
        }
        else
        {
            QPainterPath customScatterPath;
            for (int i=0; i<3; ++i)
                customScatterPath.cubicTo(qCos(2*M_PI*i/3.0)*9, qSin(2*M_PI*i/3.0)*9, qCos(2*M_PI*(i+0.9)/3.0)*9, qSin(2*M_PI*(i+0.9)/3.0)*9, 0, 0);
            customPlot->graph()->setScatterStyle(QCPScatterStyle(customScatterPath, QPen(Qt::black, 0), QColor(40, 70, 255, 50), 10));
        }
    }
    // set blank axis lines:
    customPlot->rescaleAxes();
    customPlot->xAxis->setTicks(false);
    customPlot->yAxis->setTicks(false);
    customPlot->xAxis->setTickLabels(false);
    customPlot->yAxis->setTickLabels(false);
    // make top right axes clones of bottom left axes:
    customPlot->axisRect()->setupFullAxesBox();
}

void MainWindow::setupLineStyleDemo(QCustomPlot *customPlot)
{
    demoName = "Line Style Demo";
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    QPen pen;
    QStringList lineNames;
    lineNames << "lsNone" << "lsLine" << "lsStepLeft" << "lsStepRight" << "lsStepCenter" << "lsImpulse";
    // add graphs with different line styles:
    for (int i=QCPGraph::lsNone; i<=QCPGraph::lsImpulse; ++i)
    {
        customPlot->addGraph();
        pen.setColor(QColor(qSin(i*1+1.2)*80+80, qSin(i*0.3+0)*80+80, qSin(i*0.3+1.5)*80+80));
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setName(lineNames.at(i-QCPGraph::lsNone));
        customPlot->graph()->setLineStyle((QCPGraph::LineStyle)i);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
        // generate data:
        QVector<double> x(15), y(15);
        for (int j=0; j<15; ++j)
        {
            x[j] = j/15.0 * 5*3.14 + 0.01;
            y[j] = 7*qSin(x[j])/x[j] - (i-QCPGraph::lsNone)*5 + (QCPGraph::lsImpulse)*5 + 2;
        }
        customPlot->graph()->setData(x, y);
        customPlot->graph()->rescaleAxes(true);
    }
    // zoom out a bit:
    customPlot->yAxis->scaleRange(1.1, customPlot->yAxis->range().center());
    customPlot->xAxis->scaleRange(1.1, customPlot->xAxis->range().center());
    // set blank axis lines:
    customPlot->xAxis->setTicks(false);
    customPlot->yAxis->setTicks(true);
    customPlot->xAxis->setTickLabels(false);
    customPlot->yAxis->setTickLabels(true);
    // make top right axes clones of bottom left axes:
    customPlot->axisRect()->setupFullAxesBox();
}

void MainWindow::setupParametricCurveDemo(QCustomPlot *customPlot)
{
    demoName = "Parametric Curves Demo";

    // create empty curve objects and add them to customPlot:
    QCPCurve *fermatSpiral1 = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    QCPCurve *fermatSpiral2 = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    QCPCurve *deltoidRadial = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(fermatSpiral1);
    customPlot->addPlottable(fermatSpiral2);
    customPlot->addPlottable(deltoidRadial);
    // generate the curve data points:
    int pointCount = 500;
    QVector<double> x1(pointCount), y1(pointCount);
    QVector<double> x2(pointCount), y2(pointCount);
    QVector<double> x3(pointCount), y3(pointCount);
    for (int i=0; i<pointCount; ++i)
    {
        double phi = (i/(double)(pointCount-1))*8*M_PI;
        x1[i] = qSqrt(phi)*qCos(phi);
        y1[i] = qSqrt(phi)*qSin(phi);
        x2[i] = -x1[i];
        y2[i] = -y1[i];
        double t = i/(double)(pointCount-1)*2*M_PI;
        x3[i] = 2*qCos(2*t)+qCos(1*t)+2*qSin(t);
        y3[i] = 2*qSin(2*t)-qSin(1*t);
    }
    // pass the data to the curves:
    fermatSpiral1->setData(x1, y1);
    fermatSpiral2->setData(x2, y2);
    deltoidRadial->setData(x3, y3);
    // color the curves:
    fermatSpiral1->setPen(QPen(Qt::blue));
    fermatSpiral1->setBrush(QBrush(QColor(0, 0, 255, 20)));
    fermatSpiral2->setPen(QPen(QColor(255, 120, 0)));
    fermatSpiral2->setBrush(QBrush(QColor(255, 120, 0, 30)));
    QRadialGradient radialGrad(QPointF(310, 180), 200);
    radialGrad.setColorAt(0, QColor(170, 20, 240, 100));
    radialGrad.setColorAt(0.5, QColor(20, 10, 255, 40));
    radialGrad.setColorAt(1,QColor(120, 20, 240, 10));
    deltoidRadial->setPen(QPen(QColor(170, 20, 240)));
    deltoidRadial->setBrush(QBrush(radialGrad));
    // set some basic customPlot config:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->rescaleAxes();
}

void MainWindow::setupBarChartDemo(QCustomPlot *customPlot)
{
    demoName = "Bar Chart Demo";
    // create empty bar chart objects:
    QCPBars *regen = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    QCPBars *nuclear = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    QCPBars *fossil = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(regen);
    customPlot->addPlottable(nuclear);
    customPlot->addPlottable(fossil);
    // set names and colors:
    QPen pen;
    pen.setWidthF(1.2);
    fossil->setName("Fossil fuels");
    pen.setColor(QColor(255, 131, 0));
    fossil->setPen(pen);
    fossil->setBrush(QColor(255, 131, 0, 50));
    nuclear->setName("Nuclear");
    pen.setColor(QColor(1, 92, 191));
    nuclear->setPen(pen);
    nuclear->setBrush(QColor(1, 92, 191, 50));
    regen->setName("Regenerative");
    pen.setColor(QColor(150, 222, 0));
    regen->setPen(pen);
    regen->setBrush(QColor(150, 222, 0, 70));
    // stack bars ontop of each other:
    nuclear->moveAbove(fossil);
    regen->moveAbove(nuclear);

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
    labels << "USA" << "Japan" << "Germany" << "France" << "UK" << "Italy" << "Canada";
    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickLabels(false);
    customPlot->xAxis->setTickVector(ticks);
    customPlot->xAxis->setTickVectorLabels(labels);
    customPlot->xAxis->setTickLabelRotation(60);
    customPlot->xAxis->setSubTickCount(0);
    customPlot->xAxis->setTickLength(0, 4);
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->xAxis->setRange(0, 8);

    // prepare y axis:
    customPlot->yAxis->setRange(0, 12.1);
    customPlot->yAxis->setPadding(5); // a bit more space to the left border
    customPlot->yAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
    customPlot->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    customPlot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    customPlot->yAxis->grid()->setSubGridPen(gridPen);

    // Add data:
    QVector<double> fossilData, nuclearData, regenData;
    fossilData  << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2 << 0.90*4.2 << 0.67*11.2;
    nuclearData << 0.08*10.5 << 0.12*5.5 << 0.12*5.5 << 0.40*5.8 << 0.09*5.2 << 0.00*4.2 << 0.07*11.2;
    regenData   << 0.06*10.5 << 0.05*5.5 << 0.04*5.5 << 0.06*5.8 << 0.02*5.2 << 0.07*4.2 << 0.25*11.2;
    fossil->setData(ticks, fossilData);
    nuclear->setData(ticks, nuclearData);
    regen->setData(ticks, regenData);

    // setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    customPlot->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    customPlot->legend->setBorderPen(legendPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void MainWindow::setupItemDemo(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    QMessageBox::critical(this, "", "You're using Qt < 4.7, the animation of the item demo needs functions that are available with Qt 4.7 to work properly");
#endif

    demoName = "Item Demo";
    bracketPlot = customPlot;

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPGraph *graph = customPlot->addGraph();
    int n = 500;
    double phase = 0;
    double k = 3;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; ++i)
    {
        x[i] = i/(double)(n-1)*34 - 17;
        y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
    }
    graph->setData(x, y);
    graph->setPen(QPen(Qt::blue));
    graph->rescaleKeyAxis();
    customPlot->yAxis->setRange(-1.45, 1.65);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    // add the bracket at the top:
    QCPItemBracket *bracket = new QCPItemBracket(customPlot);
    customPlot->addItem(bracket);
    bracket->left->setCoords(-8, 1.1);
    bracket->right->setCoords(8, 1.1);
    bracket->setLength(13);

    // add the text label at the top:
    QCPItemText *wavePacketText = new QCPItemText(customPlot);
    customPlot->addItem(wavePacketText);
    wavePacketText->position->setParentAnchor(bracket->center);
    wavePacketText->position->setCoords(0, -10); // move 10 pixels to the top from bracket center anchor
    wavePacketText->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
    wavePacketText->setText("Wavepacket");
    wavePacketText->setFont(QFont(font().family(), 10));

    // add the phase tracer (red circle) which sticks to the graph data (and gets updated in bracketDataSlot by timer event):
    QCPItemTracer *phaseTracer = new QCPItemTracer(customPlot);
    customPlot->addItem(phaseTracer);
    itemDemoPhaseTracer = phaseTracer; // so we can access it later in the bracketDataSlot for animation
    phaseTracer->setGraph(graph);
    phaseTracer->setGraphKey((M_PI*1.5-phase)/k);
    phaseTracer->setInterpolating(true);
    phaseTracer->setStyle(QCPItemTracer::tsCircle);
    phaseTracer->setPen(QPen(Qt::red));
    phaseTracer->setBrush(Qt::red);
    phaseTracer->setSize(7);

    // add label for phase tracer:
    QCPItemText *phaseTracerText = new QCPItemText(customPlot);
    customPlot->addItem(phaseTracerText);
    phaseTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    phaseTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignBottom);
    phaseTracerText->position->setCoords(1.0, 0.95); // lower right corner of axis rect
    phaseTracerText->setText("Points of fixed\nphase define\nphase velocity vp");
    phaseTracerText->setTextAlignment(Qt::AlignLeft);
    phaseTracerText->setFont(QFont(font().family(), 9));
    phaseTracerText->setPadding(QMargins(8, 0, 0, 0));

    // add arrow pointing at phase tracer, coming from label:
    QCPItemCurve *phaseTracerArrow = new QCPItemCurve(customPlot);
    customPlot->addItem(phaseTracerArrow);
    phaseTracerArrow->start->setParentAnchor(phaseTracerText->left);
    phaseTracerArrow->startDir->setParentAnchor(phaseTracerArrow->start);
    phaseTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    phaseTracerArrow->end->setParentAnchor(phaseTracer->position);
    phaseTracerArrow->end->setCoords(10, 10);
    phaseTracerArrow->endDir->setParentAnchor(phaseTracerArrow->end);
    phaseTracerArrow->endDir->setCoords(30, 30);
    phaseTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    phaseTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (phaseTracerText->bottom->pixelPoint().y()-phaseTracerText->top->pixelPoint().y())*0.85));

    // add the group velocity tracer (green circle):
    QCPItemTracer *groupTracer = new QCPItemTracer(customPlot);
    customPlot->addItem(groupTracer);
    groupTracer->setGraph(graph);
    groupTracer->setGraphKey(5.5);
    groupTracer->setInterpolating(true);
    groupTracer->setStyle(QCPItemTracer::tsCircle);
    groupTracer->setPen(QPen(Qt::green));
    groupTracer->setBrush(Qt::green);
    groupTracer->setSize(7);

    // add label for group tracer:
    QCPItemText *groupTracerText = new QCPItemText(customPlot);
    customPlot->addItem(groupTracerText);
    groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    groupTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    groupTracerText->position->setCoords(1.0, 0.20); // lower right corner of axis rect
    groupTracerText->setText("Fixed positions in\nwave packet define\ngroup velocity vg");
    groupTracerText->setTextAlignment(Qt::AlignLeft);
    groupTracerText->setFont(QFont(font().family(), 9));
    groupTracerText->setPadding(QMargins(8, 0, 0, 0));

    // add arrow pointing at group tracer, coming from label:
    QCPItemCurve *groupTracerArrow = new QCPItemCurve(customPlot);
    customPlot->addItem(groupTracerArrow);
    groupTracerArrow->start->setParentAnchor(groupTracerText->left);
    groupTracerArrow->startDir->setParentAnchor(groupTracerArrow->start);
    groupTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    groupTracerArrow->end->setCoords(5.5, 0.4);
    groupTracerArrow->endDir->setParentAnchor(groupTracerArrow->end);
    groupTracerArrow->endDir->setCoords(0, -40);
    groupTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    groupTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (groupTracerText->bottom->pixelPoint().y()-groupTracerText->top->pixelPoint().y())*0.85));

    // add dispersion arrow:
    QCPItemCurve *arrow = new QCPItemCurve(customPlot);
    customPlot->addItem(arrow);
    arrow->start->setCoords(1, -1.1);
    arrow->startDir->setCoords(-1, -1.3);
    arrow->endDir->setCoords(-5, -0.3);
    arrow->end->setCoords(-10, -0.2);
    arrow->setHead(QCPLineEnding::esSpikeArrow);

    // add the dispersion arrow label:
    QCPItemText *dispersionText = new QCPItemText(customPlot);
    customPlot->addItem(dispersionText);
    dispersionText->position->setCoords(-6, -0.9);
    dispersionText->setRotation(40);
    dispersionText->setText("Dispersion with\nvp < vg");
    dispersionText->setFont(QFont(font().family(), 10));

    // setup a timer that repeatedly calls MainWindow::bracketDataSlot:
    connect(&dataTimer2, SIGNAL(timeout()), this, SLOT(bracketDataSlot()));
    dataTimer2.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::setupStyledDemo(QCustomPlot *customPlot)
{
    demoName = "Styled Demo";

    // prepare data:
    QVector<double> x1(20), y1(20);
    QVector<double> x2(100), y2(100);
    QVector<double> x3(20), y3(20);
    QVector<double> x4(20), y4(20);
    for (int i=0; i<x1.size(); ++i)
    {
        x1[i] = i/(double)x1.size()*10;
        y1[i] = qCos(x1[i]*0.8+qSin(x1[i]*0.16+1.0))*qSin(x1[i]*0.54)+1.4;
    }
    for (int i=0; i<x2.size(); ++i)
    {
        x2[i] = i/(double)x2.size()*10;
        y2[i] = qCos(x2[i]*0.85+qSin(x2[i]*0.165+1.1))*qSin(x2[i]*0.50)+1.7;
    }
    for (int i=0; i<x3.size(); ++i)
    {
        x3[i] = i/(double)x3.size()*10;
        y3[i] = 0.05+3*(0.5+qCos(x3[i]*x3[i]*0.2+2)*0.5)/(double)(x3[i]+0.7)+qrand()/(double)RAND_MAX*0.01;
    }
    for (int i=0; i<x4.size(); ++i)
    {
        x4[i] = x3[i];
        y4[i] = (0.5-y3[i])+((x4[i]-2)*(x4[i]-2)*0.02);
    }

    // create and configure plottables:
    QCPGraph *graph1 = customPlot->addGraph();
    graph1->setData(x1, y1);
    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    graph1->setPen(QPen(QColor(120, 120, 120), 2));

    QCPGraph *graph2 = customPlot->addGraph();
    graph2->setData(x2, y2);
    graph2->setPen(Qt::NoPen);
    graph2->setBrush(QColor(200, 200, 200, 20));
    graph2->setChannelFillGraph(graph1);

    QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(bars1);
    bars1->setWidth(9/(double)x3.size());
    bars1->setData(x3, y3);
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(QColor(10, 140, 70, 160));

    QCPBars *bars2 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(bars2);
    bars2->setWidth(9/(double)x4.size());
    bars2->setData(x4, y4);
    bars2->setPen(Qt::NoPen);
    bars2->setBrush(QColor(10, 100, 50, 70));
    bars2->moveAbove(bars1);

    // move bars above graphs and grid below bars:
    customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
    customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
    graph1->setLayer("abovemain");
    customPlot->xAxis->grid()->setLayer("belowmain");
    customPlot->yAxis->grid()->setLayer("belowmain");

    // set some pens, brushes and backgrounds:
    customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    customPlot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    customPlot->axisRect()->setBackground(axisRectGradient);

    customPlot->rescaleAxes();
    customPlot->yAxis->setRange(0, 2);
}

void MainWindow::setupAdvancedAxesDemo(QCustomPlot *customPlot)
{
    demoName = "Advanced Axes Demo";

    // configure axis rect:
    customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    QCPAxisRect *wideAxisRect = new QCPAxisRect(customPlot);
    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    wideAxisRect->addAxis(QCPAxis::atLeft)->setTickLabelColor(QColor("#6050F8")); // add an extra axis on the left and color its numbers
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    customPlot->plotLayout()->addElement(0, 0, wideAxisRect); // insert axis rect in first row
    customPlot->plotLayout()->addElement(1, 0, subLayout); // sub layout in second row (grid layout will grow accordingly)
    //customPlot->plotLayout()->setRowStretchFactor(1, 2);
    // prepare axis rects that will be placed in the sublayout:
    QCPAxisRect *subRectLeft = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    QCPAxisRect *subRectRight = new QCPAxisRect(customPlot, false);
    subLayout->addElement(0, 0, subRectLeft);
    subLayout->addElement(0, 1, subRectRight);
    subRectRight->setMaximumSize(150, 150); // make bottom right axis rect size fixed 150x150
    subRectRight->setMinimumSize(150, 150); // make bottom right axis rect size fixed 150x150
    // setup axes in sub layout axis rects:
    subRectLeft->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    subRectRight->addAxes(QCPAxis::atBottom | QCPAxis::atRight);
    subRectLeft->axis(QCPAxis::atLeft)->setAutoTickCount(2);
    subRectRight->axis(QCPAxis::atRight)->setAutoTickCount(2);
    subRectRight->axis(QCPAxis::atBottom)->setAutoTickCount(2);
    subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    // synchronize the left and right margins of the top and bottom axis rects:
    QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
    subRectLeft->setMarginGroup(QCP::msLeft, marginGroup);
    subRectRight->setMarginGroup(QCP::msRight, marginGroup);
    wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
    foreach (QCPAxisRect *rect, customPlot->axisRects())
    {
        foreach (QCPAxis *axis, rect->axes())
        {
            axis->setLayer("axes");
            axis->grid()->setLayer("grid");
        }
    }

    // prepare data:
    QVector<double> x1a(20), y1a(20);
    QVector<double> x1b(50), y1b(50);
    QVector<double> x2(100), y2(100);
    QVector<double> x3, y3;
    qsrand(3);
    for (int i=0; i<x1a.size(); ++i)
    {
        x1a[i] = i/(double)(x1a.size()-1)*10-5.0;
        y1a[i] = qCos(x1a[i]);
    }
    for (int i=0; i<x1b.size(); ++i)
    {
        x1b[i] = i/(double)x1b.size()*10-5.0;
        y1b[i] = qExp(-x1b[i]*x1b[i]*0.2)*1000;
    }
    for (int i=0; i<x2.size(); ++i)
    {
        x2[i] = i/(double)x2.size()*10;
        y2[i] = qrand()/(double)RAND_MAX-0.5+y2[qAbs(i-1)];
    }
    x3 << 1 << 2 << 3 << 4;
    y3 << 2 << 2.5 << 4 << 1.5;

    // create and configure plottables:
    QCPGraph *mainGraph1 = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    mainGraph1->setData(x1a, y1a);
    mainGraph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    mainGraph1->setPen(QPen(QColor(120, 120, 120), 2));
    QCPGraph *mainGraph2 = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft, 1));
    mainGraph2->setData(x1b, y1b);
    mainGraph2->setPen(QPen(QColor("#8070B8"), 2));
    mainGraph2->setBrush(QColor(110, 170, 110, 30));
    mainGraph1->setChannelFillGraph(mainGraph2);
    mainGraph1->setBrush(QColor(255, 161, 0, 50));

    QCPGraph *graph2 = customPlot->addGraph(subRectLeft->axis(QCPAxis::atBottom), subRectLeft->axis(QCPAxis::atLeft));
    graph2->setData(x2, y2);
    graph2->setLineStyle(QCPGraph::lsImpulse);
    graph2->setPen(QPen(QColor("#FFA100"), 1.5));

    QCPBars *bars1 = new QCPBars(subRectRight->axis(QCPAxis::atBottom), subRectRight->axis(QCPAxis::atRight));
    customPlot->addPlottable(bars1);
    bars1->setWidth(3/(double)x3.size());
    bars1->setData(x3, y3);
    bars1->setPen(QPen(Qt::black));
    bars1->setAntialiased(false);
    bars1->setAntialiasedFill(false);
    bars1->setBrush(QColor("#705BE8"));
    bars1->keyAxis()->setAutoTicks(false);
    bars1->keyAxis()->setTickVector(x3);
    bars1->keyAxis()->setSubTickCount(0);

    // rescale axes according to graph's data:
    mainGraph1->rescaleAxes();
    mainGraph2->rescaleAxes();
    graph2->rescaleAxes();
    bars1->rescaleAxes();
    wideAxisRect->axis(QCPAxis::atLeft, 1)->setRangeLower(0);
}

void MainWindow::trainClossNN()
{
    connect(handler.get(), &UIHandler::iterationFinished,
            this, &MainWindow::onTrainIterationFinished);
    auto h = handler.get();
    connect(&dataTimer, &QTimer::timeout,
            this, [h] {
        h->requestPrediction();
    });
    handler->configure(LearnParam());
    handler->runAsync();
    dataTimer.start(200);
}

void MainWindow::onTrainIterationFinished(int iter, double error)
{
//    static double start = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
//    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
//    key -= start;
    ui->plotErrorLine->graph(0)->addData(iter, error);
    // remove data of lines that's outside visible range:
//    ui->plotColorMap->graph(0)->removeDataBefore(key-8);
    // rescale value (vertical) axis to fit the current data:
    ui->plotErrorLine->graph(0)->rescaleValueAxis();

    // make key axis range scroll with the data (at a constant range size of 8):
//    ui->plotErrorLine->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    ui->plotErrorLine->xAxis->rescale();
    ui->plotErrorLine->replot();
}

void MainWindow::bracketDataSlot()
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double secs = 0;
#else
    double secs = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif

    // update data to make phase move:
    int n = 500;
    double phase = secs*5;
    double k = 3;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; ++i)
    {
        x[i] = i/(double)(n-1)*34 - 17;
        y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
    }
    bracketPlot->graph()->setData(x, y);

    itemDemoPhaseTracer->setGraphKey((8*M_PI+fmod(M_PI*1.5-phase, 6*M_PI))/k);

    bracketPlot->replot();

    // calculate frames per second:
    double key = secs;
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(bracketPlot->graph(0)->data()->count())
            , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}

void MainWindow::setupPlayground()
{
    demoName = "Closs ANN";

    setupProblemPlane(ui->plotColorMap);
    setupErrorLine(ui->plotErrorLine);
}

void MainWindow::setupProblemPlane(QCustomPlot *plot)
{
    // configure axis rect:
    plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    plot->axisRect()->setupFullAxesBox(true);
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");

    // add predict map and its color scale
    auto scale = createPredictColorScale(plot);
    plot->plotLayout()->addElement(0, 1, scale);

    predictMap = createPredictMap(plot->xAxis, plot->yAxis, scale);
    plot->addPlottable(predictMap);

    // add training data scatter
    setupTrainingGraph(plot);

    // add testing data scatter
    setupTestingGraph(plot);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    plot->rescaleAxes();
}

QCPColorScale *MainWindow::createPredictColorScale(QCustomPlot *plot)
{
    auto scale = new QCPColorScale(plot);
    scale->setDataRange(QCPRange(-1, 1));
    scale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    scale->axis()->setLabel("Prediction Result");
    scale->setGradient(QCPColorGradient::gpPolar);

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(plot);
    plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    scale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    return scale;
}

QCPColorMap *MainWindow::createPredictMap(QCPAxis *xAxis, QCPAxis *yAxis, QCPColorScale *scale)
{
    int nx = 30;
    int ny = 30;

    // set up the QCPColorMap for prediction plane
    auto map = new QCPColorMap(xAxis, yAxis);
    // we want the color map to have nx * ny data points
    map->data()->setSize(nx, ny);
    // and span the coordinate range -1..1 in both key (x) and value (y) dimensions
    map->data()->setRange(QCPRange(0, 1), QCPRange(0, 1));

    // connect to color scale
    map->setColorScale(scale);

    // connect to data change signal
    connect(handler.get(), &UIHandler::predictionUpdated,
            [=](auto data){
        for (auto item : data) {
            auto point = item.toList();
            int x, y;
            map->data()->coordToCell(point[0].toDouble(), point[2].toDouble(), &x, &y);
            map->data()->setCell(x, y, point[1].toDouble());
        }
        map->parentPlot()->replot();
    });

    return map;
}

const int TotalClassLabel = 2;
const double labels[TotalClassLabel] = { -1.0, 1.0 };
class DataUpdater
{
    QList<QCPGraph*> series;

public:
    DataUpdater(const QList<QCPGraph*> s) : series(s)
    { }

    void operator ()(QVariantList data)
    {
        for (auto graph : series) {
            graph->clearData();
        }

        for (auto item : data) {
            auto point = item.toList();
            auto z = point[1].toDouble();
            QCPGraph *graph;
            for (int i = 0; i!= TotalClassLabel; i++) {
                qDebug() << "z =" << z << "label is" << labels[i];
                if (qFuzzyCompare(z, labels[i])) {
                    graph = series[i];
                    break;
                }
            }
            if (graph) {
                graph->addData(point[0].toDouble(), point[2].toDouble());
            }
        }
        if (!series.isEmpty()) {
            // only need to request replot on one graph
            series[0]->parentPlot()->replot();
        }
    }
};

void MainWindow::setupTrainingGraph(QCustomPlot *plot)
{
    const QColor colors[TotalClassLabel] = { Qt::blue, Qt::red };

    QPen pen;
    QList<QCPGraph *> series;
    series.reserve(TotalClassLabel);
    for (int i = 0; i!= TotalClassLabel; i++)
    {
        auto graph = plot->addGraph();
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCPScatterStyle::ssCircle);
        pen.setColor(colors[i]);
        graph->setPen(pen);
        series.append(graph);
    }
    // connect to data change signal
    connect(handler.get(), &UIHandler::trainingDataUpdated,
            DataUpdater(series));
}

void MainWindow::setupTestingGraph(QCustomPlot *plot)
{
    const QColor colors[TotalClassLabel] = { Qt::blue, Qt::red };

    QPen pen;
    QList<QCPGraph *> series;
    series.reserve(TotalClassLabel);
    for (int i = 0; i!= TotalClassLabel; i++)
    {
        auto graph = plot->addGraph();
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCPScatterStyle::ssTriangle);
        pen.setColor(colors[i]);
        graph->setPen(pen);
        series.append(graph);
    }
    // connect to data change signal
    connect(handler.get(), &UIHandler::testingDataUpdated,
            DataUpdater(series));
}

void MainWindow::setupErrorLine(QCustomPlot *plot)
{
    auto graph = plot->addGraph();
    graph->setPen(QPen(Qt::blue));
    graph->setBrush(QBrush(QColor(240, 255, 200)));
    graph->setAntialiasedFill(false);

    plot->axisRect()->setupFullAxesBox();
    // make left and bottom axes transfer their ranges to right and top axes:
//    connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis2, SLOT(setRange(QCPRange)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::screenShot()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QPixmap pm = QPixmap::grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#else
    QPixmap pm = qApp->primaryScreen()->grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#endif
    QString fileName = demoName.toLower()+".png";
    fileName.replace(" ", "");
    pm.save("./screenshots/"+fileName);
    qApp->quit();
}
