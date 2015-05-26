#include "ClossNet.h"
#include "InterruptableLMA.h"
#include "uihandler.h"
#include "models/learnparam.h"
#include "models/learntask.h"
#include "models/ucwdataset.h"
#include <OpenANN/OpenANN>
#include <OpenANN/io/Logger.h>
#include <OpenANN/util/Random.h>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QVariantMap>
#include <QReadLocker>
#include <QWriteLocker>

using namespace OpenANN;

UIHandler::UIHandler(QObject *parent)
    : QObject(parent)
    , cancelFlag(false)
    , predictionInRequest(false)
    , running(false)
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

    task = new LearnTask(param);

    configured_ = true;

    sendInputRangeUpdated();
    sendOutputRangeUpdated();
    sendTrainingDataUpdated();
    sendTestingDataUpdated();
    requestPrediction(false);
}

bool UIHandler::configured() const
{
    return configured_;
}

void UIHandler::runAsync()
{
    if (!configured_) return;
    if (running) return;

    running = true;
    auto future = QtConcurrent::run(this, &UIHandler::run);
    futureWatcher.setFuture(future);
}

void UIHandler::run()
{
    if (!configured_) return;

    // set random seed
    RandomNumberGenerator().seed(task->parameters().randSeed());
    // ensure data is in training mode before initialize
    task->data().inTrainingMode(true);

    task->network().initialize();

    InterruptableLMA opt;

    opt.setOptimizable(task->network());
    opt.setStopCriteria(task->stopCriteria());
//    opt.optimize();
    while(opt.step())
    {
        OPENANN_DEBUG << "Iteration #" << opt.currentIteration()
                      << ", training error = "
                      << OpenANN::FloatingPointFormatter(opt.currentError(), 4);
        emit iterationFinished(task, opt.currentIteration(), opt.currentError());
//        {
//            QReadLocker locker(&lockForCancelFlag);
//            if(cancelFlag) break;
//        }
    }
    opt.result();
}

void UIHandler::terminateTraining()
{
    if (!configured_) return;
    QWriteLocker locker(&lockForCancelFlag);
    cancelFlag = true;
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

void UIHandler::onTrainingFinished()
{
    cancelFlag = false;
    running = false;
}

void UIHandler::onIterationFinished()
{
    generatePrediction(task->network());
}

void UIHandler::generatePrediction(Learner& learner)
{
    QVariantList prediction;
    for(int x = 0; x < 30; x++)
    {
        for(int y = 0; y < 30; y++)
        {
            double xx = x / 30.0;
            double yy = y / 30.0;
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
    emit inputRangeUpdated(task->data().inputRange().first, task->data().inputRange().second);
}

void UIHandler::sendOutputRangeUpdated()
{
    if (!configured_) return;
    emit outputRangeUpdated(task->data().outputRange().first,
                            task->data().outputRange().second,
                            task->data().outputLabelCount());
}
