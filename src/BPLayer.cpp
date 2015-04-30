#include "BPLayer.h"
#include <OpenANN/util/Random.h>

using namespace OpenANN;

BPLayer::BPLayer(OutputInfo info, int J, bool bias,
                 ActivationFunction act, double stdDev)
    : nInput(info.outputs()), nUnits(J), hasBias(bias), act(act), stdDev(stdDev),
      weight(nUnits, nInput), dWeight(nUnits, nInput),
      prevOutput(0), net(1, nUnits),
      output(1, nUnits), dAct(1, nUnits),
      delta(1, nUnits), prevDelta(1, nUnits),
      bias(nUnits), dBias(nUnits)
{
}

OutputInfo BPLayer::initialize(std::vector<double*>& parameterPointers,
                               std::vector<double*>& parameterDerivativePointers)
{
    parameterPointers.reserve(parameterPointers.size() + nUnits * (nInput + hasBias));
    parameterDerivativePointers.reserve(parameterDerivativePointers.size() + nUnits * (nInput + hasBias));
    for(int j = 0; j < nUnits; j++)
    {
        for(int i = 0; i < nInput; i++)
        {
            parameterPointers.push_back(&weight(j, i));
            parameterDerivativePointers.push_back(&dWeight(j, i));
        }
        if(hasBias)
        {
            parameterPointers.push_back(&bias(j));
            parameterDerivativePointers.push_back(&dBias(j));
        }
    }

    initializeParameters();

    OutputInfo info;
    info.dimensions.push_back(nUnits);
    return info;
}

void BPLayer::initializeParameters()
{
    RandomNumberGenerator rng;
    rng.fillNormalDistribution(weight, stdDev);
    if(hasBias)
        rng.fillNormalDistribution(bias, stdDev);
}

void BPLayer::updatedParameters()
{
}

void BPLayer::forwardPropagate(Eigen::MatrixXd* prevOutput, Eigen::MatrixXd*& output, bool, double*)
{
    const int nPattern = prevOutput->rows();
    this->output.conservativeResize(nPattern, Eigen::NoChange);
    this->prevOutput = prevOutput;
    // Combine inputs to scalar
    net = *prevOutput * weight.transpose();
    if(hasBias)
        net.rowwise() += bias.transpose();
    // Compute output
    activationFunction(act, net, this->output);
    output = &(this->output);
}

void BPLayer::backpropagate(Eigen::MatrixXd* deltaIn, Eigen::MatrixXd*& deltaOut,
                            bool backpropToPrevious)
{
    const int nPattern = net.rows();
    dAct.conservativeResize(nPattern, Eigen::NoChange);
    // Derive activations
    activationFunctionDerivative(act, net, dAct);
    delta = dAct.cwiseProduct(*deltaIn);
    // Weight derivatives
    dWeight = delta.transpose() **prevOutput;
    if(hasBias)
        dBias = delta.colwise().sum().transpose();
    // Prepare error signals for previous layer
    if(backpropToPrevious)
        prevDelta = delta * weight;
    deltaOut = &prevDelta;
}

Eigen::MatrixXd& BPLayer::getOutput()
{
    return output;
}

Eigen::VectorXd BPLayer::getParameters()
{
    Eigen::VectorXd p(nUnits * (nInput + hasBias));
    int idx = 0;
    for(int j = 0; j < nUnits; j++)
        for(int i = 0; i < nInput; i++)
            p(idx++) = weight(j, i);
    if(hasBias)
        for(int j = 0; j < nUnits; j++)
            p(idx++) = bias(j);
    return p;
}
