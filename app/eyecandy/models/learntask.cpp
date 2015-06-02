#include "learntask.h"
#include "models/ucwdataset.h"
#include "ClossNet.h"
#include "OpenANN/OpenANN"

LearnTask::LearnTask(const LearnParam &param)
{
    // save param in case later use
    param_ = param;

    // Step 1. setup stopping criteria
    stopCriteria_ = make_unique(new StoppingCriteria(param.stoppingCriteria()));

    // Step 2. setup data source
    data_ = make_unique(createDataSourceFromParam(param));

    // Step 3. setup network
    switch (param.errorFunc()) {
    case LearnParam::MSE:
        network_ = make_unique(new Net);
        break;
    case LearnParam::Closs:
        network_ = make_unique(new ClossNet);
        // set parameters
        clossNet().setKernelSize(param.kernelSize());
        clossNet().setPValue(param.pValue());
        clossNet().setLearningRate(param.learningRate());
        break;
    }

    // initialize MLP
    for (auto layer : param.layers()) {
        switch (layer.type) {
        case LayerDesc::Input:
            network_->inputLayer(data_->inputs());
            break;
        case LayerDesc::FullyConnected:
            network_->fullyConnectedLayer(layer.nUnit, (ActivationFunction)layer.activationFunc);
            break;
        case LayerDesc::Output:
            network_->outputLayer(data_->outputs(), (ActivationFunction)layer.activationFunc);
            break;
        default:
            break;
        }
    }
    network_->trainingSet(*data_);
    network_->initialize();
}

UCWDataSet *LearnTask::createDataSourceFromParam(const LearnParam &param)
{
    auto source = new UCWDataSet(UCWDataSet::None);
    switch (param.dataSource()) {
    case UCWDataSet::TwoSpirals:
        source->generateTwoSpirals();
        break;
    case UCWDataSet::CSV:
        source->generateCSV(param.csvFilePath());
        break;
    default:
        break;
    }
    return source;
}

Net &LearnTask::network()
{
    return *network_;
}

ClossNet &LearnTask::clossNet()
{
    return *static_cast<ClossNet*>(network_.get());
}

StoppingCriteria &LearnTask::stopCriteria()
{
    return *stopCriteria_;
}

UCWDataSet &LearnTask::data()
{
    return *data_;
}

const LearnParam &LearnTask::parameters() const
{
    return param_;
}
