#include "learnparam.h"
#include <QDebug>

LearnParam::LearnParam(double learnRate, double kernelSize, double pValue)
    : dataSource_(TwoSprial)
    , learningRate_(learnRate)
    , kernelSize_(kernelSize)
    , pValue_(pValue)
{
    stoppingCriteria_.maximalIterations = 10000;
    layers_ << LayerDesc{LayerDesc::Input, 0, LayerDesc::LINEAR} // Input
            << LayerDesc{LayerDesc::FullyConnected, 20, LayerDesc::TANH}
            << LayerDesc{LayerDesc::FullyConnected, 20, LayerDesc::TANH}
            << LayerDesc{LayerDesc::Output, 0, LayerDesc::TANH}; // Output
}


double LearnParam::learningRate() const
{
    return learningRate_;
}

void LearnParam::learningRate(double value)
{
    learningRate_ = value;
}

double LearnParam::kernelSize() const
{
    return kernelSize_;
}

void LearnParam::kernelSize(double value)
{
    kernelSize_ = value;
}

double LearnParam::pValue() const
{
    return pValue_;
}

void LearnParam::pValue(double value)
{
    pValue_ = value;
}

LearnParam::DataSource LearnParam::dataSource() const
{
    return dataSource_;
}

void LearnParam::dataSource(DataSource source)
{
    dataSource_ = source;
}

const StoppingCriteria &LearnParam::stoppingCriteria() const
{
    return stoppingCriteria_;
}

void LearnParam::stoppingCriteria(const StoppingCriteria &criteria)
{
    stoppingCriteria_ = criteria;
}

const QList<LayerDesc> &LearnParam::layers() const
{
    return layers_;
}

QList<LayerDesc> &LearnParam::layers()
{
    return layers_;
}

void LearnParam::layers(const QList<LayerDesc> &l)
{
    layers_ = l;
}

void LearnParam::ensureHasOutputLayer()
{
    if (layers_.isEmpty()) {
        layers_.append({
            LayerDesc::FullyConnected,
            0,
            LayerDesc::TANH,
        });
    }
}

void LearnParam::debugPrint() const
{
    qDebug() << "Learning rate:" << learningRate();
    qDebug() << "Kernel size:" << kernelSize();
    qDebug() << "P value:" << pValue();
    qDebug() << "Layers:";
    for (auto layer : layers()) {
        qDebug() << "    " << "Type:" << layer.type
                 << "nUnit:" << layer.nUnit
                 << "ActFunc:" << layer.activationFunc;
    }
}
