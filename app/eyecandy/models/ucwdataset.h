#ifndef DATASET_H_
#define DATASET_H_

#include <Eigen/Core>
#include <OpenANN/io/DataSet.h>
#include <QObject>
#include <QMutex>
#include <QVariantList>
#include <memory>
#include <utility>

namespace OpenANN {
class DirectStorageDataSet;
}
using Eigen::MatrixXd;
using OpenANN::DataSet;
using OpenANN::DirectStorageDataSet;
using OpenANN::Learner;
using std::unique_ptr;
using std::pair;
using range = pair<double, double>;

class UCWDataSet : public QObject, public DataSet
{
    Q_OBJECT
public:
    class ContextManager
    {
        UCWDataSet *dataset_;
        bool oldMode_;
        bool multiThreadSafe_;
        bool empty_;

        ContextManager(UCWDataSet *dataset_, bool inTrainingMode, bool multiThreadSafe = true);
        void maybeLock();
        void maybeUnlock();

        friend class UCWDataSet;
    public:
        ContextManager(ContextManager &&other);
        ~ContextManager();
    };

    enum DataSource {
        TwoSpirals,
        CSV,
        None,
    };

    UCWDataSet(DataSource source = TwoSpirals);
    virtual ~UCWDataSet();

    virtual int samples();
    virtual int inputs();
    virtual int outputs();
    virtual Eigen::VectorXd& getInstance(int i);
    virtual Eigen::VectorXd& getTarget(int i);
    virtual void finishIteration(Learner& learner);

    void inTrainingMode(bool val);
    bool inTrainingMode() const;
    ContextManager enterTrainingMode(bool multiThreadSafe = true);
    ContextManager enterTestingMode(bool multiThreadSafe = true);

    range inputRange() const;
    range outputRange() const;
    int outputLabelCount() const;

    /**
     * Creates dataset csv file.
     * with following constrain:
     * nInput + nOutput <= 10
     *
     * @param filePath path to the csv file
     * @param nInput number of input units
     * @param nOutput number of output units
     */
    bool generateCSV(QString filePath, int nInput = 2, int nOutput = 1);

    /**
     * Creates two interlocked spirals that form different classes.
     *
     * Source is available at
     * <a href="http://www.cs.cmu.edu/afs/cs/project/ai-repository/ai/areas/neural/bench/0.html">
     * CMU Neural Networks Benchmark Collection</a>
     *
     * @param density higher densities result in greater data sets
     * @param maxDiameter diameter of the points on the outer side of the spirals
     */
    bool generateTwoSpirals(int density = 2, double maxDiameter = 1.0);

    bool generateNone();

signals:

protected:
    DirectStorageDataSet &dataSet();
    void createInternalDataSet();
    void dispose();

private:
    bool inTrainingMode_;
    MatrixXd trainingIn;
    MatrixXd trainingOut;
    MatrixXd testingIn;
    MatrixXd testingOut;
    unique_ptr<DirectStorageDataSet> trainingData;
    unique_ptr<DirectStorageDataSet> testingData;
    range inputRange_;
    range outputRange_;
    int outputLabelCount_;
    QMutex modeLock_;
};
#endif // DATASET_H_
