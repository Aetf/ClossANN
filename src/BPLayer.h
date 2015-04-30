#ifndef BPLAYER_H_
#define BPLAYER_H_

#include <OpenANN/layers/Layer.h>
#include <OpenANN/ActivationFunctions.h>

using OpenANN::ActivationFunction;
using OpenANN::Layer;
using OpenANN::OutputInfo;

/**
 * @class BPLayer
 *
 * Fully connected layer used in BP ANN.
 *
 * Each neuron in the previous layer is taken as input for each neuron of this
 * layer. Forward propagation is usually done by \f$ a = W \cdot x + b, y =
 * g(a) \f$, where \f$ a \f$ is the activation vector, \f$ y \f$ is the
 * output, \f$ g \f$ a typically nonlinear activation function that operates
 * on a vector, \f$ x \f$ is the input of the layer, \f$ W \f$ is a weight
 * matrix and \f$ b \f$ is a bias vector.
 */
class BPLayer : public Layer
{
protected:
    int nInput, nUnits;
    bool hasBias;
    ActivationFunction act;
    double stdDev;

    // weight matrix. nUnits rows and nInputs cols
    Eigen::MatrixXd weight;
    Eigen::MatrixXd dWeight;

    // pointer to previous layer's output. nInput cols, each row is a pattern
    Eigen::MatrixXd* prevOutput;

    // act func input. nUnits cols, each row is a pattern
    Eigen::MatrixXd net;

    // output matrix. nUnits cols, each row is a pattern
    Eigen::MatrixXd output;

    // derivation of act func.
    Eigen::MatrixXd dAct;

    // deltas used in bp
    Eigen::MatrixXd delta;
    // store deltaOut for previous layer
    Eigen::MatrixXd prevDelta;

    // bias vector
    Eigen::VectorXd bias;
    Eigen::VectorXd dBias;

public:
    BPLayer(OutputInfo info, int nUnits, bool hasBias, ActivationFunction act, double stdDev);
    virtual OutputInfo initialize(std::vector<double*>& parameterPointers,
                                  std::vector<double*>& parameterDerivativePointers);
    virtual void initializeParameters();
    virtual void updatedParameters();
    virtual void forwardPropagate(Eigen::MatrixXd* prevOutput, Eigen::MatrixXd*& output, bool, double*);
    virtual void backpropagate(Eigen::MatrixXd* deltaIn, Eigen::MatrixXd*& deltaOut,
                               bool backpropToPrevious);
    virtual Eigen::MatrixXd& getOutput();
    virtual Eigen::VectorXd getParameters();
};

#endif // BPLAYER_H_
