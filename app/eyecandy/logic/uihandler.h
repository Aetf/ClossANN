#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <QObject>
#include <QReadWriteLock>
#include <QFutureWatcher>
#include <QVariantList>

class ClossNet;
class LearnTask;
class LearnParam;
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
    void configure(const LearnParam &param);
    void dispose();
    void test();
    void requestPrediction();

    QVariantList getTrainingSet();
    QVariantList getTestingSet();

signals:
    void predictionUpdated(QVariantList data);
    void trainingDataUpdated(QVariantList data);
    void testingDataUpdated(QVariantList data);
    void iterationFinished(int iter, double error);

protected slots:
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

    // async task handling
    QFutureWatcher<void> futureWatcher;
};

#endif // UIHANDLER_H
