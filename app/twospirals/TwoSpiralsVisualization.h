#ifndef TWO_SPIRALS_VISUALIZATION_H_
#define TWO_SPIRALS_VISUALIZATION_H_

#include <OpenANN/OpenANN>
#include <OpenANN/io/DirectStorageDataSet.h>
#include <Eigen/Core>
#include <QGLWidget>
#include <QKeyEvent>
#include <QMutex>
#include "ClossNet.h"

using namespace OpenANN;

class TwoSpiralsVisualization;

class TwoSpiralsDataSet : public DataSet
{
    Eigen::MatrixXd in, out;
    DirectStorageDataSet dataSet;
    TwoSpiralsVisualization* visualization;
public:
    TwoSpiralsDataSet(const Eigen::MatrixXd& inputs, const Eigen::MatrixXd& outputs);
    void setVisualization(TwoSpiralsVisualization* visualization);
    virtual ~TwoSpiralsDataSet() {}
    virtual int samples() {
        return dataSet.samples();
    }
    virtual int inputs() {
        return dataSet.inputs();
    }
    virtual int outputs() {
        return dataSet.outputs();
    }
    virtual Eigen::VectorXd& getInstance(int i) {
        return dataSet.getInstance(i);
    }
    virtual Eigen::VectorXd& getTarget(int i) {
        return dataSet.getTarget(i);
    }
    virtual void finishIteration(Learner& learner);
};

class TwoSpiralsVisualization : public QGLWidget
{
    Q_OBJECT
    int width, height;
    QMutex classesMutex;
    double classes[100][100];
    TwoSpiralsDataSet trainingSet;
    TwoSpiralsDataSet testSet;
    bool showTraining, showTest, showPrediction, showSmooth;
    Net* net;
    StoppingCriteria stop;

public:
    TwoSpiralsVisualization(const Eigen::MatrixXd& trainingInput, const Eigen::MatrixXd& trainingOutput,
                            const Eigen::MatrixXd& testInput, const Eigen::MatrixXd& testOutput);
    virtual ~TwoSpiralsVisualization();
    void predictClass(int x, int y, double predictedClass);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();
    void paintPrediction();
    void paintDataSet(bool training);
    virtual void keyPressEvent(QKeyEvent* keyEvent);

signals:
    void updatedData();
};

#endif // TWO_SPIRALS_VISUALIZATION_H_
