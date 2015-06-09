#include "ClossNet.h"
#include "InterruptableLMA.h"
#include "uihandler.h"
#include "models/learnparam.h"
#include "models/learntask.h"
#include "models/ucwdataset.h"
#include "utils/logger.h"
#include <OpenANN/OpenANN>
#include <OpenANN/io/Logger.h>
#include <OpenANN/util/Random.h>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QVariantMap>
#include <QReadLocker>
#include <QWriteLocker>
#include <QMutexLocker>

using OpenANN::RandomNumberGenerator;

UIHandler::UIHandler(QObject *parent)
    : QObject(parent)
    , cancelFlag(false)
    , predictionInRequest(false)
    , running_(false)
    , configured_(false)
    , task(nullptr)
{
    connect(this, &UIHandler::iterationFinished,
            this, &UIHandler::onIterationFinished);
    connect(&futureWatcher, &QFutureWatcher<void>::finished,
            this, &UIHandler::onTrainingFinished);
}

UIHandler::~UIHandler()
{
    dispose();
}

void UIHandler::dispose()
{
    terminateTraining();
    delete task;
    task = nullptr;
    configured_ = false;
}

QObject *UIHandler::UIHandlerProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new UIHandler;
}

void UIHandler::configure(const LearnParam &param)
{
    if (configured_) dispose();

    Log::info() << "正在配置网络...";
    Log::info() << "网络参数：<br>" << param;

    task = new LearnTask(param);

    configured_ = true;

    sendInputRangeUpdated();
    sendOutputRangeUpdated();
    sendTrainingDataUpdated();
    sendTestingDataUpdated();

    if (!param.disablePredict)
        requestPrediction(false);
}

void UIHandler::runAsync()
{
    if (!configured_) return;
    if (running_) return;

    running_ = true;
    cancelFlag = false;
    auto future = QtConcurrent::run(this, &UIHandler::run);
    futureWatcher.setFuture(future);
}

void UIHandler::run()
{
    if (!configured_) return;

    // set random seed
    RandomNumberGenerator().seed(task->parameters().randSeed());

    Log::normal() << "网络初始化...";
    task->network().initialize();
    Log::normal() << "网络初始化完成";

    InterruptableLMA opt;

    opt.setOptimizable(task->network());
    opt.setStopCriteria(task->stopCriteria());

    // protect multithread access to data
    auto step = [&]() {
        // ensure data is in training mode
        auto ctx = task->data().enterTrainingMode(false);
        return opt.step();
    };

    while(step())
    {
        Log::info() << "第" << opt.currentIteration() << "次迭代，"
                    << "Error = "
                    << QStringLiteral("%1").arg(opt.currentError(), 0, 'g', 4);

        double testError = 0.0;
        // compute testing error, ensure in testing mode
        auto ctx = task->data().enterTestingMode(false);
        task->network().trainingSet(task->data());
        if (task->data().samples() > 0) {
            testError = task->network().error();
        }

        emit iterationFinished(task, opt.currentIteration(), opt.currentError(), testError);

        QReadLocker locker(&lockForCancelFlag);
        if(cancelFlag) {
            Log::normal() << "训练中途取消";
            break;
        }
    }
    opt.result();

    computeMeanMisclassificationPossibility();
}

void UIHandler::onTrainingFinished()
{
    cancelFlag = false;
    running_ = false;
    emit trainingStopped();
}

void UIHandler::terminateTraining()
{
    if (!configured() || !training()) return;
    {
        QWriteLocker locker(&lockForCancelFlag);
        cancelFlag = true;
    }
    futureWatcher.waitForFinished();
}

void UIHandler::requestPrediction(bool async)
{
    if (!configured_) return;

    if (async) {
        predictionInRequest = true;
    } else {
        generatePrediction(task->network());
    }
}

void UIHandler::requestPredictionAsync()
{
    requestPrediction(true);
}

QVariantList UIHandler::getTrainingSet()
{
    QVariantList data;
    bool oldMode = task->data().inTrainingMode();
    task->data().inTrainingMode(true);
    int n = task->data().samples();
    for (int i = 0; i!= n; i++)
    {
        auto in = task->data().getInstance(i);
        auto out = task->data().getTarget(i);

        QVariantList point;
        point << in(0) << out(0) << in(1);
        data << QVariant(point);
    }

    task->data().inTrainingMode(oldMode);
    return data;
}

QVariantList UIHandler::getTestingSet()
{
    QVariantList data;
    bool oldMode = task->data().inTrainingMode();
    task->data().inTrainingMode(false);
    int n = task->data().samples();
    for (int i = 0; i!= n; i++)
    {
        auto in = task->data().getInstance(i);
        auto out = task->data().getTarget(i);

        QVariantList point;
        point << in(0) << out(0) << in(1);
        data << QVariant(point);
    }

    task->data().inTrainingMode(oldMode);
    return data;
}

void UIHandler::onIterationFinished()
{
    if (predictionInRequest)
        generatePrediction(task->network());
}

void UIHandler::generatePrediction(Learner& learner)
{
    int n = 30;
    auto range = task->data().inputRange();
    auto step = (range.second - range.first) / n;
    QVariantList prediction;
    for(int x = 0; x < n; x++)
    {
        for(int y = 0; y < n; y++)
        {
            double xx = range.first + step * x;
            double yy = range.first + step * y;
            Eigen::VectorXd in(2);
            in << xx, yy;
            Eigen::VectorXd out = learner(in);
            QVariantList point;
            point << xx << out(0, 0) << yy;
            prediction << QVariant(point);
        }
    }
    emit predictionUpdated(prediction);
}

void UIHandler::computeMeanMisclassificationPossibility()
{
    auto ctx = task->data().enterTestingMode();
    int correct = task->data().samples();
    if (!correct) return;

    for (int i = 0; i!= task->data().samples(); i++) {
        auto in = task->data().getInstance(i);
        auto desired = task->data().getTarget(i);
        auto out = task->network()(in);

        if (!match(out, desired)) --correct;
    }
    Log::normal() << "在测试集上的误分类率为 "
                  << (100.0 - correct * 100.0 / task->data().samples())
                  << "%";
}

void UIHandler::sendTrainingDataUpdated()
{
    if (!configured_) return;
    emit trainingDataUpdated(getTrainingSet());
}

void UIHandler::sendTestingDataUpdated()
{
    if (!configured_) return;
    emit testingDataUpdated(getTestingSet());
}

void UIHandler::sendInputRangeUpdated()
{
    if (!configured_) return;
    emit inputRangeUpdated(task->data().inputRange().first,
                           task->data().inputRange().second);
}

void UIHandler::sendOutputRangeUpdated()
{
    if (!configured_) return;
    emit outputRangeUpdated(task->data().outputRange().first,
                            task->data().outputRange().second,
                            task->data().outputLabelCount());
}
