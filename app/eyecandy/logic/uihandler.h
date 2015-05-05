#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <QObject>
#include <QReadWriteLock>
#include <QFutureWatcher>
#include <QVariantList>

class ClossNet;
class LearnTask;
class QQmlEngine;
class QJSEngine;
namespace OpenANN {
class Learner;
}
using OpenANN::Learner;

class UIHandler : public QObject
{
    Q_OBJECT
public:
    explicit UIHandler(QObject *parent = 0);
    virtual ~UIHandler();
    static QObject *UIHandlerProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

public slots:
    void runAsync();
    void run();
    void terminateTraining();
    void configure();
    void dispose();
    void test();

    QVariantList getPrediction();
    QVariantList getTrainingSet();
    QVariantList getTestingSet();

signals:
    void predictionUpdated(QVariantList data);
    void trainingDataUpdated(QVariantList data);
    void testingDataUpdated(QVariantList data);

protected slots:
    void onUnderlayPredictionUpdated(QVariantList data);
    void onTrainingFinished();

private:
    void sendTrainingDataUpdated();
    void sendTestingDataUpdated();

    // multi-threading
    QReadWriteLock lockForCancelFlag;
    bool cancelFlag;
    bool running;

    // neural network related
    bool configured;
    LearnTask *task;
    QVariantList thePrediction;

    // async task handling
    QFutureWatcher<void> futureWatcher;
};

#endif // UIHANDLER_H
