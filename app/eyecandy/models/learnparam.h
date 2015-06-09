#ifndef LEARNPARAM_H
#define LEARNPARAM_H

#include <QList>
#include <QMetaType>
#include <OpenANN/optimization/StoppingCriteria.h>
#include "models/ucwdataset.h"
#include "utils/logger.h"

using OpenANN::StoppingCriteria;
using DataSource = UCWDataSet::DataSource;

struct LayerDesc
{
    enum LayerType {
        Input,
        FullyConnected,
        Output,
        Unknown
    };
    enum ActivationFunction {
        LOGISTIC = 0,
        TANH = 1,
        TANH_SCALED = 2,
        RECTIFIER = 3,
        LINEAR = 4,
        SOFTMAX = 4,
        Unknwon
    };

    LayerType type = FullyConnected;
    int nUnit = 1;
    ActivationFunction activationFunc = TANH;
};

Q_DECLARE_METATYPE(LayerDesc)

class LearnParam
{
public:
    enum ErrorFunction{
        MSE,
        Closs
    };

    LearnParam(double learningRate = 1, double kernelSize = 0.5, double pValue = 2);

    ErrorFunction errorFunc() const;
    LearnParam& errorFunc(ErrorFunction func);

    double learningRate() const;
    LearnParam& learningRate(double value);

    double kernelSize() const;
    LearnParam& kernelSize(double value);

    double pValue() const;
    LearnParam& pValue(double value);

    unsigned int randSeed() const;
    LearnParam& randSeed(unsigned int seed);

    /**
     * The last element in list is used as output layer, whose nUnit is determined by input dataset,
     * thus its LayerDesc::nUnit is ignored. So there must be at least one element in the list.
     * @return
     */
    const QList<LayerDesc> &layers() const;
    QList<LayerDesc> &layers();
    void layers(const QList<LayerDesc> &l);

    DataSource dataSource() const;
    LearnParam& dataSource(DataSource source);

    QString csvFilePath() const;
    LearnParam& csvFilePath(const QString &path);

    const StoppingCriteria &stoppingCriteria() const;
    LearnParam& stoppingCriteria(const StoppingCriteria &criteria);

    void ensureHasOutputLayer();

    void debugPrint() const;
    QString toDebugString(int indent = 4, char indentChar = ' ') const;

    bool disablePredict;
private:
    DataSource dataSource_;
    QString csvFilePath_;

    StoppingCriteria stoppingCriteria_;

    ErrorFunction errorFunc_;
    double learningRate_;
    double kernelSize_;
    double pValue_;
    unsigned int randSeed_;
    QList<LayerDesc> layers_;
};

Logger operator <<(Logger writter, const LearnParam &param);

#endif // LEARNPARAM_H
