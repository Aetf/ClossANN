#ifndef DATASET_H_
#define DATASET_H_

#include <Eigen/Core>
#include <OpenANN/io/DataSet.h>
#include <QObject>
#include <QVariantList>
#include <memory>

namespace OpenANN {
class DirectStorageDataSet;
}
using Eigen::MatrixXd;
using OpenANN::DataSet;
using OpenANN::DirectStorageDataSet;
using OpenANN::Learner;
using std::unique_ptr;

class UCWDataSet : public QObject, public DataSet
{
    Q_OBJECT
public:
    enum DataSource {
        TwoSpirals,
        Moon,
    };

    UCWDataSet(DataSource source = TwoSpirals, int density = 2, double maxDiameter = 1.0);
    virtual ~UCWDataSet();

    virtual int samples();
    virtual int inputs();
    virtual int outputs();
    virtual Eigen::VectorXd& getInstance(int i);
    virtual Eigen::VectorXd& getTarget(int i);
    virtual void finishIteration(Learner& learner);

    void inTrainingMode(bool val);
    bool inTrainingMode() const;

    /**
     * Creates moon shape dataset that form two classes.
     *
     * @param density higher densities result in greater data sets
     * @param maxDiameter diameter of the points on the outer side of the spirals
     * @param trainInputs training inputs
     * @param trainOutputs training outputs
     * @param testInputs test inputs
     * @param testOutputs test outputs
     */
    void generateMoon(int density, double maxDiameter);

    /**
     * Creates two interlocked spirals that form different classes.
     *
     * Source is available at
     * <a href="http://www.cs.cmu.edu/afs/cs/project/ai-repository/ai/areas/neural/bench/0.html">
     * CMU Neural Networks Benchmark Collection</a>
     *
     * @param density higher densities result in greater data sets
     * @param maxDiameter diameter of the points on the outer side of the spirals
     * @param trainInputs training inputs
     * @param trainOutputs training outputs
     * @param testInputs test inputs
     * @param testOutputs test outputs
     */
    void generateTwoSpirals(int density, double maxDiameter);

signals:
    void predictionUpdated(QVariantList);

protected:
    DirectStorageDataSet &dataSet();
    void dispose();

private:
    bool inTrainingMode_;
    MatrixXd trainingIn;
    MatrixXd trainingOut;
    MatrixXd testingIn;
    MatrixXd testingOut;
    unique_ptr<DirectStorageDataSet> trainingData;
    unique_ptr<DirectStorageDataSet> testingData;
};
#endif // DATASET_H_
