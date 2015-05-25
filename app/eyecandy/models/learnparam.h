#ifndef LEARNPARAM_H
#define LEARNPARAM_H

#include <QList>
#include <QMetaType>
#include <OpenANN/optimization/StoppingCriteria.h>

using OpenANN::StoppingCriteria;

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
    enum DataSource {
        TwoSprial
    };

    LearnParam(double learningRate = 1, double kernelSize = 0.5, double pValue = 2);

    double learningRate() const;
    void learningRate(double value);

    double kernelSize() const;
    void kernelSize(double value);

    double pValue() const;
    void pValue(double value);

    /**
     * The last element in list is used as output layer, whose nUnit is determined by input dataset,
     * thus its LayerDesc::nUnit is ignored. So there must be at least one element in the list.
     * @return
     */
    const QList<LayerDesc> &layers() const;
    QList<LayerDesc> &layers();
    void layers(const QList<LayerDesc> &l);

    DataSource dataSource() const;
    void dataSource(DataSource source);

    const StoppingCriteria &stoppingCriteria() const;
    void stoppingCriteria(const StoppingCriteria &criteria);

    void ensureHasOutputLayer();

    void debugPrint() const;

private:
    DataSource dataSource_;

    StoppingCriteria stoppingCriteria_;

    double learningRate_;
    double kernelSize_;
    double pValue_;
    QList<LayerDesc> layers_;
};

#endif // LEARNPARAM_H
