#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <QObject>
#include <QReadWriteLock>
#include <QFutureWatcher>
#include <QVariantList>

class ClossNet;
class LearnTask;
class LearnParam;

QT_BEGIN_NAMESPACE
class QQmlEngine;
class QJSEngine;
QT_END_NAMESPACE

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
    void requestPrediction(bool async = true);
    void requestPredictionAsync();

    inline bool configured() const { return configured_; }
    inline bool training() const { return running_; }

    QVariantList getTrainingSet();
    QVariantList getTestingSet();

signals:
    void predictionUpdated(QVariantList data);
    void trainingDataUpdated(QVariantList data);
    void testingDataUpdated(QVariantList data);
    void iterationFinished(LearnTask *task, int iter, double error, double testError);
    void inputRangeUpdated(double min, double max);
    void outputRangeUpdated(double min, double max, int labelsCount);
    void trainingStopped();

protected slots:
    void onTrainingFinished();
    void onIterationFinished();
    void generatePrediction(Learner& learner);

private:
    void sendTrainingDataUpdated();
    void sendTestingDataUpdated();
    void sendInputRangeUpdated();
    void sendOutputRangeUpdated();

    // multi-threading
    QReadWriteLock lockForCancelFlag;
    bool cancelFlag;
    bool predictionInRequest;
    bool running_;

    // neural network related
    bool configured_;
    LearnTask *task;

    // async task handling
    QFutureWatcher<void> futureWatcher;
};

#endif // UIHANDLER_H
