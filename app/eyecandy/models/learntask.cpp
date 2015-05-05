#include "learntask.h"
#include "models/ucwdataset.h"
#include "ClossNet.h"
#include "OpenANN/OpenANN"

using namespace OpenANN;

LearnTask::LearnTask()
{
    network_ = make_unique(new ClossNet);
    stopCriteria_ = make_unique(new StoppingCriteria);
    data_ = make_unique(new UCWDataSet);

    // initialize MLP
    network_->inputLayer(data_->inputs())
    // use this as only hidden layer to try extreme learning machine
    //.extremeLayer(1500, RECTIFIER, 1.0)
    .fullyConnectedLayer(20, TANH)
    .fullyConnectedLayer(20, TANH)
//    net->bpLayer(20, TANH)
//    .bpLayer(20, TANH)
    .outputLayer(data_->outputs(), TANH)
    .trainingSet(*data_);
    network_->initialize();

    // set stop criteria
    stopCriteria_->maximalIterations = 10000;
    stopCriteria_->minimalSearchSpaceStep = 1e-10;
    stopCriteria_->minimalValueDifferences = 1e-10;
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
