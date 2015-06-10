#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <QDebug>
#include <QAction>
#include <QDesktopWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFileDialog>
#include <QAbstractButton>
#include <QTabWidget>
#include <QScreen>
#include <QList>
#include <QMessageBox>
#include <QMetaEnum>
#include <qcustomplot.h>
#include <OpenANN/util/Random.h>
#include <Eigen/Eigen>
#include "logic/uihandler.h"
#include "models/layerdescmodel.h"
#include "models/learntask.h"
#include "widgets/layerdelegate.h"
#include "widgets/loglistwidget.h"
#include "utils/utils.h"
#include "utils/awesomeiconprovider.h"
#include "utils/logger.h"
#include "columnresizer.h"
#include "ClossNet.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , handler(new UIHandler)
    , layersModel(nullptr)
    , predictMap(nullptr)
    , trainingGraph(nullptr)
    , testingGraph(nullptr)
    , disablePredict(false)
{
    ui->setupUi(this);
    setupToolbar();
    setupOptionPage();
    setupMonitorPage();
    setupLogPage();
    setupFuncPage();

    if (!disablePredict)
        connect(&predictionTimer, &QTimer::timeout,
                handler.get(),&UIHandler::requestPredictionAsync);

    Log::normal() << "窗口初始化完成";
}

void MainWindow::setupToolbar()
{
    ui->actionTrain->setIcon(AwesomeIconProvider::instance()->icon(fa::play));
    connect(ui->actionTrain, &QAction::triggered,
            this, &MainWindow::trainClossNN);
    ui->toolBar->addAction(ui->actionTrain);

    ui->actionStop->setIcon(AwesomeIconProvider::instance()->icon(fa::stop));
    connect(ui->actionStop, &QAction::triggered,
            this, &MainWindow::stopTraining);
    ui->toolBar->addAction(ui->actionStop);
}

void MainWindow::setupOptionPage()
{
    auto colResizer = new ColumnResizer(this);
    colResizer->addWidgetsFromLayout(ui->groupNet->layout(), 0);
    colResizer->addWidgetsFromLayout(ui->groupCloss->layout(), 0);
    colResizer->addWidgetsFromLayout(ui->groupData->layout(), 0);

    // Group Net
    ui->btnRefreshSeed->setIcon(AwesomeIconProvider::instance()->icon(fa::refresh));
    connect(ui->spinLearnRate, Select<double>::OverloadOf(&QDoubleSpinBox::valueChanged),
    this, [=](auto value) {
        currentParam.learningRate(value);
    });
    connect(ui->lineRandSeed, &QLineEdit::textChanged,
    [=](auto text) {
        currentParam.randSeed(text.toUInt());
    });
    connect(ui->btnRefreshSeed, &QAbstractButton::clicked,
    this, [=] {
        ui->lineRandSeed->setText(QString::number(get_seed()));
    });
    connect(ui->comboErrorFunc, Select<int>::OverloadOf(&QComboBox::currentIndexChanged),
    this, [=]() {
        auto func = LearnParam::ErrorFunction(ui->comboErrorFunc->currentData().toInt());
        currentParam.errorFunc(func);

        auto enableCloss = (func == LearnParam::Closs);
        ui->groupCloss->setEnabled(enableCloss);
    });
    ui->comboErrorFunc->addItem("Closs", LearnParam::Closs);
    ui->comboErrorFunc->addItem("MSE", LearnParam::MSE);

    // Group Closs
    connect(ui->spinKernelSize, Select<double>::OverloadOf(&QDoubleSpinBox::valueChanged),
    this, [=](auto value) {
        currentParam.kernelSize(value);
    });
    connect(ui->spinPValue, Select<double>::OverloadOf(&QDoubleSpinBox::valueChanged),
    this, [=](auto value) {
        currentParam.pValue(value);
    });

    // Group Data
    connect(ui->comboDataSource, Select<int>::OverloadOf(&QComboBox::currentIndexChanged),
    this, [=]() {
        auto source = DataSource(ui->comboDataSource->currentData().toInt());
        currentParam.dataSource(source);

        auto enableCSV = (source == DataSource::CSV);
        ui->lineCSVFile->setEnabled(enableCSV);
        ui->btnCSVBrowse->setEnabled(enableCSV);
    });
    ui->comboDataSource->addItem("TwoSpirals", DataSource::TwoSpirals);
    ui->comboDataSource->addItem("CSV", DataSource::CSV);

    connect(ui->lineCSVFile, &QLineEdit::textChanged,
    this, [=](auto text) {
        currentParam.csvFilePath(text);
    });
    connect(ui->btnCSVBrowse, &QAbstractButton::clicked,
    this, [=]() {
        auto path = QFileDialog::getOpenFileName(this, tr("打开CSV数据文件"),
                    "",
                    tr("CSV 文件(*.csv)"));
        if (!path.isEmpty())
            ui->lineCSVFile->setText(path);
    });

    // Group Net Structure
    layersModel = new LayerDescModel(this);
    ui->tableNetStru->setModel(layersModel);
    ui->tableNetStru->setItemDelegate(new LayerDelegate(this));
    ui->tableNetStru->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableNetStru->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableNetStru->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->tableNetStru->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::updateButtons);

    connect(ui->btnInsertLayer, &QAbstractButton::clicked,
    this, [=] {
        auto pos = ui->tableNetStru->selectionModel()->currentIndex().row();
        layersModel->insertLayer(pos + 1, LayerDesc());
    });
    connect(ui->btnRemoveLayer, &QAbstractButton::clicked,
    this, [=] {
        auto pos = ui->tableNetStru->selectionModel()->currentIndex().row();
        layersModel->removeRows(pos, 1);
    });

    displayDefaultOptions();
}

void MainWindow::setupLogPage()
{
    auto msgList = static_cast<LogListWidget*>(ui->listLog);
    auto addLogMessage = [msgList](const Log::Msg &msg) {
        QString text;
        QDateTime time = QDateTime::fromMSecsSinceEpoch(msg.timestamp);
        QColor color;

        switch (msg.type) {
        case Log::INFO:
            color.setNamedColor("blue");
            break;
        case Log::WARNING:
            color.setNamedColor("orange");
            break;
        case Log::CRITICAL:
            color.setNamedColor("red");
            break;
        default:
            color = QApplication::palette().color(QPalette::WindowText);
        }

        text = "<font color='grey'>"
               + time.toString(Qt::SystemLocaleShortDate)
               + "</font> - <font color='" + color.name() + "'>"
               + msg.message + "</font>";
        msgList->appendLine(text);
    };

    auto store = Log::LogStorage::instance();
    for (auto msg : store->getMessages()) {
        addLogMessage(msg);
    }
    connect(store, &Log::LogStorage::newLogMessage, this, addLogMessage);
}

void MainWindow::setupMonitorPage()
{
    if (!disablePredict)
        setupProblemPlane(ui->plotColorMap);
    setupErrorLine(ui->plotErrorLine);
}

void MainWindow::applyOptions()
{
    currentParam.layers(layersModel->layers());
    currentParam.disablePredict = disablePredict;

    Log::normal() << "应用参数...";
    handler->configure(currentParam);
}

void MainWindow::displayDefaultOptions()
{
    // Group Net
    ui->spinLearnRate->setValue(currentParam.learningRate());
    ui->lineRandSeed->setText(QString::number(currentParam.randSeed()));
    ui->comboErrorFunc->setCurrentIndex(
                ui->comboErrorFunc->findData(currentParam.errorFunc()));

    // Group Closs
    ui->spinKernelSize->setValue(currentParam.kernelSize());
    ui->spinPValue->setValue(currentParam.pValue());

    // Group Data
    ui->comboDataSource->setCurrentIndex(
                ui->comboDataSource->findData(currentParam.dataSource()));
    ui->lineCSVFile->setText(currentParam.csvFilePath());

    // Group Net Structure
    layersModel->removeRows(0, layersModel->rowCount());
    layersModel->insertRows(0, currentParam.layers().size());
    for (int i = 0; i!= currentParam.layers().size(); i++) {
        QVariant v;
        v.setValue(currentParam.layers()[i]);
        layersModel->setData(layersModel->index(i, 0), v, LayerDescModel::LayerDataRole);
    }
    ui->tableNetStru->selectRow(0);
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

void MainWindow::trainClossNN()
{
    applyOptions();
    lastIterTime = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    handler->runAsync();
    if (!disablePredict)
        predictionTimer.start(200);
}

void MainWindow::stopTraining()
{
    if (!disablePredict)
        predictionTimer.stop();
    handler->terminateTraining();
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
    // and span the coordinate range 0..1 in both key (x) and value (y) dimensions
    map->data()->setRange(QCPRange(0, 1), QCPRange(0, 1));

    // connect to color scale
    map->setColorScale(scale);

    // connect to data change signal
    connect(handler.get(), &UIHandler::predictionUpdated,
    [=](auto data) {
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

class DataUpdater
{
    QList<pair<double,QCPGraph*>> graphAndLabels;
    range labelRange;

public:
    DataUpdater(const QList<QCPGraph*> s, range labelRange)
        : labelRange(labelRange)
    {
        int labelCount = s.size();

        double step = (labelRange.second - labelRange.first) / (labelCount - 1);
        double l = labelRange.first;
        for (int i = 0; i!= labelCount; i++) {
            graphAndLabels.append({l, s[i]});
            l += step;
        }
    }

    void operator ()(QVariantList data)
    {
        for (auto item : graphAndLabels) {
            item.second->clearData();
        }

        for (auto item : data) {
            auto point = item.toList();
            auto z = point[1].toDouble();
            QCPGraph *graph;
            for (auto item : graphAndLabels) {
                if (qFuzzyCompare(z, item.first)) {
                    graph = item.second;
                    break;
                }
            }
            if (graph) {
                graph->addData(point[0].toDouble(), point[2].toDouble());
            }
        }
        if (!graphAndLabels.isEmpty()) {
            // only need to request replot on one graph
            graphAndLabels[0].second->parentPlot()->replot();
        }
    }
};

void MainWindow::setupTrainingGraph(QCustomPlot *plot, range outputRange, int labelsCount)
{
    const int ColorCount = 2;
    const QColor colors[ColorCount] = { Qt::blue, Qt::red };

    QPen pen;
    QList<QCPGraph *> series;
    series.reserve(labelsCount);
    for (int i = 0; i!= labelsCount; i++)
    {
        auto graph = plot->addGraph();
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCPScatterStyle::ssCircle);
        pen.setColor(colors[i%ColorCount]);
        graph->setPen(pen);
        series.append(graph);
    }

    // try disconnect from any previous signal first
    disconnect(handler.get(), &UIHandler::trainingDataUpdated, nullptr, nullptr);

    connect(handler.get(), &UIHandler::trainingDataUpdated,
            DataUpdater(series, outputRange));
}

void MainWindow::setupTestingGraph(QCustomPlot *plot, range outputRange, int labelsCount)
{
    const int ColorCount = 2;
    const QColor colors[ColorCount] = { Qt::blue, Qt::red };

    QPen pen;
    QList<QCPGraph *> series;
    series.reserve(labelsCount);
    for (int i = 0; i!= labelsCount; i++)
    {
        auto graph = plot->addGraph();
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCPScatterStyle::ssTriangle);
        pen.setColor(colors[i%ColorCount]);
        graph->setPen(pen);
        series.append(graph);
    }

    // try disconnect from any previous signal first
    disconnect(handler.get(), &UIHandler::testingDataUpdated, nullptr, nullptr);

    connect(handler.get(), &UIHandler::testingDataUpdated,
            DataUpdater(series, outputRange));
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

    // postpone two scatters setup to after applying options
    connect(handler.get(), &UIHandler::inputRangeUpdated,
    this, [=](auto min, auto max) {
        QCPRange range(min, max);
        predictMap->data()->setRange(range, range);
        plot->xAxis->setRange(range);
        plot->yAxis->setRange(range);
    });

    connect(handler.get(), &UIHandler::outputRangeUpdated,
    this, [=](auto min, auto max, auto labelsCount) {
        // set prediction map data range
        scale->setDataRange(QCPRange(min, max));

        // (re)setup scatters
        plot->clearGraphs();
        setupTrainingGraph(plot, {min, max}, labelsCount);
        setupTestingGraph(plot, {min, max}, labelsCount);
    });
}

void MainWindow::setupErrorLine(QCustomPlot *plot)
{
    // Step 1: configure axis rect
    // clear default axis rect so we can start from scratch
    plot->plotLayout()->clear();
    auto wideAxisRect = new QCPAxisRect(plot);
    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atLeft)->setAutoTickCount(4);
    wideAxisRect->axis(QCPAxis::atLeft)->setRange(-1, 105);

    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    // insert axis rect in first row
    plot->plotLayout()->addElement(0, 0, wideAxisRect);
    // sub layout in second row (grid layout will grow accordingly)
    plot->plotLayout()->addElement(1, 0, subLayout);
    plot->plotLayout()->setRowStretchFactor(0, 2);
    // prepare axis rects that will be placed in the sublayout:
    auto subRectLeft = new QCPAxisRect(plot, false); // false means to not setup default axes
    auto subRectRight = new QCPAxisRect(plot, false);
    subLayout->addElement(0, 0, subRectLeft);
    subLayout->addElement(0, 1, subRectRight);
    // make bottom left axis rect size fixed 150x150
    subRectLeft->setMaximumSize(150, 150);
    // setup axes in sub layout axis rects:
    subRectLeft->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    subRectRight->addAxes(QCPAxis::atBottom | QCPAxis::atRight);
    subRectLeft->axis(QCPAxis::atLeft)->setAutoTickCount(3);
    subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subRectRight->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    // synchronize the left and right margins of the top and bottom axis rects:
    auto marginGroup = new QCPMarginGroup(plot);
    subRectLeft->setMarginGroup(QCP::msLeft, marginGroup);
    subRectRight->setMarginGroup(QCP::msRight, marginGroup);
    wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
    for (auto rect : plot->axisRects())
    {
        for (auto axis : rect->axes())
        {
            axis->setLayer("axes");
            axis->grid()->setLayer("grid");
        }
    }
    // add legend
    auto legend = new QCPLegend;
    wideAxisRect->insetLayout()->addElement(legend, Qt::AlignRight | Qt::AlignTop);
    plot->legend = legend;
    plot->setAutoAddPlottableToLegend(false);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // Step 2: create and configure plottables
    auto graphTrain = plot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graphTrain->setPen(QPen(QColor(110, 170, 110), 2));
    graphTrain->setBrush(QColor(110, 170, 110, 50));
    graphTrain->setName("训练集正确率");
    graphTrain->addToLegend();

    auto graphTesting = plot->addGraph(wideAxisRect->axis(QCPAxis::atBottom),
                                       wideAxisRect->axis(QCPAxis::atLeft));
    graphTesting->setPen(QPen(QColor(255, 161, 0)));
    graphTesting->setBrush(QColor(255, 161, 0, 50));
    graphTesting->setChannelFillGraph(graphTrain);
    graphTesting->setName("测试集正确率");
    graphTesting->addToLegend();

    auto graphTime = plot->addGraph(subRectLeft->axis(QCPAxis::atBottom),
                                   subRectLeft->axis(QCPAxis::atLeft));
    graphTime->setLineStyle(QCPGraph::lsImpulse);
    graphTime->setPen(QPen(QColor("#FFA100"), 1.5));

    auto graphError = plot->addGraph(subRectRight->axis(QCPAxis::atBottom),
                                  subRectRight->axis(QCPAxis::atRight));
    graphError->setPen(QPen(QColor(1, 92, 191)));
    graphError->setBrush(QColor(1, 92, 191, 50));

    // connect to data signals
    connect(handler.get(), &UIHandler::iterationFinished,
    this, [=](auto, auto iter, auto error, auto trainRate, auto testRate) {
        static double lastIpsTime;
        static int iterCount;
        if (iter == 0) {
            graphTrain->clearData();
            graphTesting->clearData();
            graphTime->clearData();
            graphError->clearData();
            iterCount = 0;
            lastIpsTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        }

        // timing
        double now = QDateTime::currentDateTime().toMSecsSinceEpoch();
        graphTime->addData(iter, now - lastIterTime);
        graphTime->removeDataBefore(iter - 50);
        lastIterTime = now;

        ++iterCount;
        if ((now - lastIpsTime)/1000.0 > 2) // average ips over 2 seconds
        {
            ui->statusBar->showMessage(
                    QString("%1 IPS, Training Epochs: %2")
                        .arg(iterCount/(now - lastIpsTime)*1000.0, 0, 'f', 0)
                        .arg(iter)
                    , 0);
            lastIpsTime = now;
            iterCount = 0;
        }

        // training error
        graphTrain->addData(iter, trainRate);
        graphTesting->addData(iter, testRate);
        graphError->addData(iter, error);

        // rescale axis to fit the current data:
        wideAxisRect->axis(QCPAxis::atBottom)->rescale();
        subRectLeft->axis(QCPAxis::atLeft)->rescale();
        subRectLeft->axis(QCPAxis::atBottom)->rescale();
        subRectRight->axis(QCPAxis::atRight)->rescale();
        subRectRight->axis(QCPAxis::atBottom)->rescale();

        // zoom out a bit
        wideAxisRect->axis(QCPAxis::atBottom)->scaleRange(1.05, 0);
        wideAxisRect->axis(QCPAxis::atBottom)->moveRange(0.25);

        plot->replot();
    });
    connect(handler.get(), &UIHandler::trainingStopped,
    this, [=]{
        lastIterTime = -1;
    });
}

void MainWindow::setupFuncPage()
{
    auto plot1 = ui->plotFunc1;
    plot1->legend->setVisible(true);
    plot1->setAutoAddPlottableToLegend(true);
    plot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    auto plot2 = ui->plotFunc2;
    plot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPGraph *graph1[4];
    for (int i = 0; i!= 4; i++) {
        graph1[i] = plot1->addGraph();
    }
    graph1[0]->setPen(QPen(QColor(0, 255, 0), 2));
    graph1[0]->setName("closs sigma = 0.5");
    graph1[1]->setPen(QPen(QColor(255, 0, 0), 2));
    graph1[1]->setName("closs sigma = 1");
    graph1[2]->setPen(QPen(QColor(0, 0, 255), 2));
    graph1[2]->setName("closs sigma = 2");
    graph1[3]->setPen(QPen(QColor(0, 0, 0), 2));
    graph1[3]->setName("0-1 loss");

    QCPGraph *graph2[3];
    for (int i = 0; i!= 3; i++) {
        graph2[i] = plot2->addGraph();
        graph2[i]->setPen(QPen(QColor(110, 170, 11 * i), 2));
        graph2[i]->setBrush(QColor(110, 170, 110, 50));
    }

    auto closs = [](const auto &YmT, double kernelSize, double pValue) {
        double lambda = -1 / (2 * kernelSize * kernelSize);
        double beta = 1 / (1 - exp(lambda));
        Eigen::MatrixXd rbf = (YmT.array().abs().pow(pValue) * lambda).exp();

        auto err = (beta * (1 - rbf.array())).eval();
        return err;
    };

    auto dcloss = [](const auto& x, double kernelSize, double pValue) {
        double lambda = -1 / (2 * kernelSize * kernelSize);
        double beta = 1 / (1 - exp(lambda));
        Eigen::MatrixXd rbf = (x.array().abs().pow(pValue) * lambda).exp().eval();

        auto sign = [](double x) { return x >= 0 ? 1 : -1; };
        auto signx = x.unaryExpr(sign);

        auto tmp = x.array().abs().pow(pValue-1).matrix().cwiseProduct(signx);

        auto d = (beta * (-lambda) * pValue * rbf * tmp).eval();
        return d;
    };

    auto loss01 = [](double x){
        return x <= 0 ? 1 : 0;
    };


    QVector<double> x, ycloss05, ydcloss, ycloss1, ycloss2, y01loss;
    double step = 0.01;
    for (double k = -1; k <= 1; k+=step) {
        x << k;
        Eigen::MatrixXd vx(1, 1);
        vx << (1-k);
        ycloss05 << closs(vx, 0.5, 2)(0, 0);
        ycloss1 << closs(vx, 0.5, 1)(0, 0);
        ycloss2 << closs(vx, 0.5, 5.5)(0, 0);
        y01loss << loss01(k);
    }
    x.clear();
    for (double k = 0; k <= 2; k+=step) {
        x << k;
        Eigen::MatrixXd vx(1, 1);
        vx << k;
        ydcloss << dcloss(vx, 0.5, 5.5)(0, 0);
    }

    graph1[0]->setData(x, ycloss05);
    graph1[1]->setData(x, ycloss1);
    graph1[2]->setData(x, ycloss2);
    graph1[3]->setData(x, y01loss);
    graph2[0]->setData(x, ydcloss);
    plot1->axisRect()->setupFullAxesBox();
    plot2->axisRect()->setupFullAxesBox();
    plot1->rescaleAxes();
    plot2->rescaleAxes();
    plot2->yAxis->setRangeUpper(4);
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
