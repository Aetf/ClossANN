#include "learnparam.h"
#include <QDebug>
#include <QTextStream>
#include "utils/utils.h"

LearnParam::LearnParam(double learnRate, double kernelSize, double pValue)
    : dataSource_(DataSource::CSV)
    , csvFilePath_("/media/Documents/GradProject/data/VQdata.csv")
    , learningRate_(learnRate)
    , kernelSize_(kernelSize)
    , pValue_(pValue)
    , randSeed_(get_seed())
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

LearnParam &LearnParam::learningRate(double value)
{
    learningRate_ = value;
    return *this;
}

double LearnParam::kernelSize() const
{
    return kernelSize_;
}

LearnParam &LearnParam::kernelSize(double value)
{
    kernelSize_ = value;
    return *this;
}

double LearnParam::pValue() const
{
    return pValue_;
}

LearnParam &LearnParam::pValue(double value)
{
    pValue_ = value;
    return *this;
}

unsigned int LearnParam::randSeed() const
{
    return randSeed_;
}

LearnParam &LearnParam::randSeed(unsigned int seed)
{
    randSeed_ = seed;
    return *this;
}

DataSource LearnParam::dataSource() const
{
    return dataSource_;
}

LearnParam &LearnParam::dataSource(DataSource source)
{
    dataSource_ = source;
    return *this;
}

QString LearnParam::csvFilePath() const
{
    return csvFilePath_;
}

LearnParam& LearnParam::csvFilePath(const QString &path)
{
    csvFilePath_ = path;
    return *this;
}

const StoppingCriteria &LearnParam::stoppingCriteria() const
{
    return stoppingCriteria_;
}

LearnParam &LearnParam::stoppingCriteria(const StoppingCriteria &criteria)
{
    stoppingCriteria_ = criteria;
    return *this;
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

QString LearnParam::toDebugString(int indent) const
{
    QString str;
    QString ind(indent, ' ');
    QTextStream out(&str);

    out << ind << "Data source:" << dataSource() << "\n";
    if (dataSource() == DataSource::CSV) {
        out << ind << "CSV file path:" << csvFilePath() << "\n";
    }
    out << ind << "Learning rate:" << learningRate() << "\n";
    out << ind << "Kernel size:" << kernelSize() << "\n";
    out << ind << "P value:" << pValue() << "\n";
    out << ind << "Layers:" << "\n";
    for (auto layer : layers()) {
        out << ind << "    "
            << "Type:" << layer.type
            << "nUnit:" << layer.nUnit
            << "ActFunc:" << layer.activationFunc
            << "\n";
    }

    return str;
}

void LearnParam::debugPrint() const
{
    qDebug() << toDebugString(0);
}
