#include "ClossNet.h"
#include "InterruptableLMA.h"
#include "uihandler.h"
#include "models/learntask.h"
#include "models/ucwdataset.h"
#include <OpenANN/OpenANN>
#include <OpenANN/io/Logger.h>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QVariantMap>
#include <QReadLocker>
#include <QWriteLocker>

using namespace OpenANN;

UIHandler::UIHandler(QObject *parent)
    : QObject(parent)
    , cancelFlag(false)
    , running(false)
    , configured(false)
    , task(nullptr)
{
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
    configured = false;
}

QObject *UIHandler::UIHandlerProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return new UIHandler;
}

void UIHandler::configure()
{
    if (configured) dispose();

    task = new LearnTask;
    connect(&task->data(), &UCWDataSet::predictionUpdated,
            this, &UIHandler::predictionUpdated);

    configured = true;

    sendTrainingDataUpdated();
    sendTestingDataUpdated();
}

void UIHandler::runAsync()
{
    if (!configured) return;
    if (running) return;

    running = true;
    auto future = QtConcurrent::run(this, &UIHandler::run);
    futureWatcher.setFuture(future);
}

void UIHandler::run()
{
    if (!configured) return;

    task->data().inTrainingMode(true);
    task->network().initialize();

    InterruptableLMA opt;

    opt.setOptimizable(task->network());
    opt.setStopCriteria(task->stopCriteria());
    while(opt.step())
    {
        OPENANN_DEBUG << "Iteration #" << opt.currentIteration()
                      << ", training error = "
                      << OpenANN::FloatingPointFormatter(opt.currentError(), 4);
        {
            QReadLocker locker(&lockForCancelFlag);
            if(cancelFlag) break;
        }
    }
    opt.result();
}

void UIHandler::terminateTraining()
{
    if (!configured) return;
    QWriteLocker locker(&lockForCancelFlag);
    cancelFlag = true;
}

void UIHandler::requestPrediction()
{
    if (!configured) return;
    task->data().requestPrediction();
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

void UIHandler::test()
{
    static bool odd = true;
    const int MM = 10;
    QVariantList thePrediction;
    for(int x = 0; x < MM; x++)
    {
        for(int y = 0; y < MM; y++)
        {
            QVariantList point;
            point << x / (double) MM;
            if(odd) {
                point << (y%2 != 0? -0.8 : 0.8);
            } else {
                point << (y%2 == 0? -0.8 : 0.8);
            }
            point << y / (double) MM;
            thePrediction << QVariant(point);
        }
    }
    odd = !odd;
    emit predictionUpdated(thePrediction);
}

void UIHandler::sendTrainingDataUpdated()
{
    if (!configured) return;
    emit trainingDataUpdated(getTrainingSet());
}

void UIHandler::sendTestingDataUpdated()
{
    if (!configured) return;
    emit testingDataUpdated(getTestingSet());
}