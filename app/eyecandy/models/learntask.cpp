#include "learntask.h"
#include "models/ucwdataset.h"
#include "ClossNet.h"
#include "OpenANN/OpenANN"

using namespace OpenANN;

LearnTask::LearnTask(const LearnParam &param)
{
    // save param in case later use
    param_ = param;

    // Step 1. setup stopping criteria
    stopCriteria_ = make_unique(new StoppingCriteria(param.stoppingCriteria()));

    // Step 2. setup data source
    data_ = make_unique(createDataSourceFromParam(param));

    // Step 3. setup network
    network_ = make_unique(new ClossNet);

    // set parameters
    network_->setKernelSize(param.kernelSize());
    network_->setPValue(param.pValue());
    network_->setLearningRate(param.learningRate());
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
    switch (param.dataSource()) {
    case LearnParam::TwoSprial:
        return new UCWDataSet;
    default:
        return nullptr;
    }
}

ClossNet &LearnTask::network()
{
    return *network_;
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
