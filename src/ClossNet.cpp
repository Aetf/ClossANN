#include <OpenANN/OpenANN>
#include <OpenANN/io/Logger.h>
#include <OpenANN/io/DirectStorageDataSet.h>
#include <OpenANN/util/OpenANNException.h>
#include <OpenANN/util/Random.h>
#include <Eigen/Core>
#include <iostream>

#include "ClossNet.h"
#include "BPLayer.h"

using namespace OpenANN;

/**
 * Create feedforward neural network.
 */
ClossNet::ClossNet()
    : kernelSize(0.5)
    , pValue(2.0)
{
    errorFunction = NO_E_DEFINED;
    Net::useDropout(false);
}

ClossNet& ClossNet::bpLayer(int units, ActivationFunction act, double stdDev, bool bias)
{
    architecture << "bp_layer " << units << " " << (int) act << " "
                 << stdDev << " " << bias << " ";
    addLayer(new BPLayer(infos.back(), units, bias, act, stdDev));
    return *this;
}

void ClossNet::save(std::ostream& stream)
{
    Net::save(stream);
    stream << "kernelSize " << kernelSize;
    stream << "pValue" << pValue;
}

void ClossNet::load(std::istream& stream)
{
    std::string type;
    while(!stream.eof())
    {
        stream >> type;
        if(type == "input")
        {
            int dim1, dim2, dim3;
            stream >> dim1 >> dim2 >> dim3;
            OPENANN_DEBUG << "input " << dim1 << " " << dim2 << " " << dim3;
            inputLayer(dim1, dim2, dim3);
        }
        else if(type == "alpha_beta_filter")
        {
            double deltaT, stdDev;
            stream >> deltaT >> stdDev;
            OPENANN_DEBUG << "alpha_beta_filter" << deltaT << " " << stdDev;
            alphaBetaFilterLayer(deltaT, stdDev);
        }
        else if(type == "fully_connected")
        {
            int units;
            int act;
            double stdDev;
            bool bias;
            stream >> units >> act >> stdDev >> bias;
            OPENANN_DEBUG << "fully_connected " << units << " " << act << " "
                          << stdDev << " " << bias;
            fullyConnectedLayer(units, (ActivationFunction) act, stdDev, bias);
        }
        else if(type == "bp_layer")
        {
            int units;
            int act;
            double stdDev;
            bool bias;
            stream >> units >> act >> stdDev >> bias;
            OPENANN_DEBUG << "bp_layer " << units << " " << act << " "
                          << stdDev << " " << bias;
            bpLayer(units, (ActivationFunction) act, stdDev, bias);
        }
        else if(type == "rbm")
        {
            int H;
            int cdN;
            double stdDev;
            bool backprop;
            stream >> H >> cdN >> stdDev >> backprop;
            OPENANN_DEBUG << "rbm " << H << " " << cdN << " " << stdDev << " "
                          << backprop;
            restrictedBoltzmannMachineLayer(H, cdN, stdDev, backprop);
        }
        else if(type == "sae")
        {
            int H;
            double beta, rho;
            int act;
            stream >> H >> beta >> rho >> act;
            OPENANN_DEBUG << "sae " << H << " " << beta << " " << rho << " " << act;
            sparseAutoEncoderLayer(H, beta, rho, (ActivationFunction) act);
        }
        else if(type == "compressed")
        {
            int units;
            int params;
            int act;
            std::string compression;
            double stdDev;
            bool bias;
            stream >> units >> params >> act >> compression >> stdDev >> bias;
            OPENANN_DEBUG << "compressed " << units << " " << params << " " << act
                          << " " << compression << " " << stdDev << " " << bias;
            compressedLayer(units, params, (ActivationFunction) act, compression,
                            stdDev, bias);
        }
        else if(type == "extreme")
        {
            int units;
            int act;
            double stdDev;
            bool bias;
            stream >> units >> act >> stdDev >> bias;
            OPENANN_DEBUG << "extreme " << units << " " << act << " " << stdDev
                          << " " << bias;
            extremeLayer(units, (ActivationFunction) act, stdDev, bias);
        }
        else if(type == "intrinsic_plasticity")
        {
            double targetMean;
            double stdDev;
            stream >> targetMean >> stdDev;
            OPENANN_DEBUG << "intrinsic_plasticity " << targetMean << " " << stdDev;
            intrinsicPlasticityLayer(targetMean, stdDev);
        }
        else if(type == "convolutional")
        {
            int featureMaps, kernelRows, kernelCols, act;
            double stdDev;
            bool bias;
            stream >> featureMaps >> kernelRows >> kernelCols >> act >> stdDev >> bias;
            OPENANN_DEBUG << "convolutional " << featureMaps << " " << kernelRows
                          << " " << kernelCols << " " << act << " " << stdDev << " " << bias;
            convolutionalLayer(featureMaps, kernelRows, kernelCols,
                               (ActivationFunction) act, stdDev, bias);
        }
        else if(type == "subsampling")
        {
            int kernelRows, kernelCols, act;
            double stdDev;
            bool bias;
            stream >> kernelRows >> kernelCols >> act >> stdDev >> bias;
            OPENANN_DEBUG << "subsampling " << kernelRows << " " << kernelCols
                          << " " << act << " " << stdDev << " " << bias;
            subsamplingLayer(kernelRows, kernelCols, (ActivationFunction) act,
                             stdDev, bias);
        }
        else if(type == "max_pooling")
        {
            int kernelRows, kernelCols;
            stream >> kernelRows >> kernelCols;
            OPENANN_DEBUG << "max_pooling " << kernelRows << " " << kernelCols;
            maxPoolingLayer(kernelRows, kernelCols);
        }
        else if(type == "local_response_normalization")
        {
            double k, alpha, beta;
            int n;
            stream >> k >> n >> alpha >> beta;
            OPENANN_DEBUG << "local_response_normalization " << k << " " << n << " "
                          << alpha << " " << beta;
            localReponseNormalizationLayer(k, n, alpha, beta);
        }
        else if(type == "dropout")
        {
            double dropoutProbability;
            stream >> dropoutProbability;
            OPENANN_DEBUG << "dropout " << dropoutProbability;
            dropoutLayer(dropoutProbability);
        }
        else if(type == "output")
        {
            int units;
            int act;
            double stdDev;
            bool bias;
            stream >> units >> act >> stdDev >> bias;
            OPENANN_DEBUG << "output " << units << " " << act << " " << stdDev
                          << " " << bias;
            outputLayer(units, (ActivationFunction) act, stdDev, bias);
        }
        else if(type == "compressed_output")
        {
            int units;
            int params;
            int act;
            std::string compression;
            double stdDev;
            bool bias;
            stream >> units >> params >> act >> compression >> stdDev >> bias;
            OPENANN_DEBUG << "compressed_output " << units << " " << params << " "
                          << act << " " << compression << " " << stdDev << " " << bias;
            compressedOutputLayer(units, params, (ActivationFunction) act,
                                  compression, stdDev, bias);
        }
        else if(type == "error_function")
        {
            int errorFunction;
            stream >> errorFunction;
            OPENANN_DEBUG << "error_function " << errorFunction;
            setErrorFunction((ErrorFunction) errorFunction);
        }
        else if(type == "regularization")
        {
            double l1Penalty, l2Penalty, maxSquaredWeightNorm;
            stream >> l1Penalty >> l2Penalty >> maxSquaredWeightNorm;
            OPENANN_DEBUG << "regularization " << l1Penalty << " " << l2Penalty
                          << " " << maxSquaredWeightNorm;
            setRegularization(l1Penalty, l2Penalty, maxSquaredWeightNorm);
        }
        else if(type == "parameters")
        {
            double p = 0.0;
            for(int i = 0; i < dimension(); i++)
                stream >> parameterVector(i);
            setParameters(parameterVector);
        }
        else if(type == "kernelSize")
        {
            double k = 0.0;
            stream >> k;
            setKernelSize(k);
        }
        else if(type == "pValue")
        {
            double p = 0.0;
            stream >> p;
            setPValue(p);
        }
        else
        {
            throw OpenANNException("Unknown layer type: '" + type + "'.");
        }
    }
}

Net& ClossNet::setRegularization(double l1Penalty, double l2Penalty, double maxSquaredWeightNorm)
{
    return *this;
}

Net& ClossNet::setErrorFunction(ErrorFunction errorFunction)
{
    return *this;
}

Net& ClossNet::useDropout(bool activate)
{
    return *this;
}

double ClossNet::getKernelSize() const
{
    return kernelSize;
}

ClossNet& ClossNet::setKernelSize(double kernel)
{
    kernelSize = kernel;
    return *this;
}

double ClossNet::getPValue() const
{
    return pValue;
}

ClossNet& ClossNet::setPValue(double value)
{
    pValue = value;
    return *this;
}

double ClossNet::error(unsigned int n)
{
    std::vector<int> vec;
    vec.push_back(n);
    return error(vec.begin(), vec.end()).mean();
}

Eigen::VectorXd ClossNet::error(std::vector<int>::const_iterator startN,
                                std::vector<int>::const_iterator endN)
{
    const int nPatterns = endN - startN;
    tempInput.conservativeResize(nPatterns, trainSet->inputs());
    Eigen::MatrixXd T(nPatterns, trainSet->outputs());
    int n = 0;
    for(std::vector<int>::const_iterator it = startN; it != endN; ++it, ++n)
    {
        tempInput.row(n) = trainSet->getInstance(*it);
        T.row(n) = trainSet->getTarget(*it);
    }
    forwardPropagate(nullptr);
    tempError = tempOutput - T;
    return clossFunction(tempError).rowwise().sum();
}

void ClossNet::forwardPropagate(double *error)
{
    Eigen::MatrixXd* y = &tempInput;
    for(std::vector<Layer*>::iterator layer = layers.begin();
        layer != layers.end(); ++layer)
      (**layer).forwardPropagate(y, y, dropout, error);
    tempOutput = *y;
    OPENANN_CHECK_EQUALS(y->cols(), infos.back().outputs());
    if(errorFunction == CE)
      OpenANN::softmax(tempOutput);
}

double ClossNet::error()
{
    std::vector<int> vec;
    for(int n = 0; n < N; n++)
        vec.push_back(n);
    return error(vec.begin(), vec.end()).mean();
}

bool ClossNet::providesGradient()
{
    return true;
}

void ClossNet::errorGradient(std::vector<int>::const_iterator startN,
                             std::vector<int>::const_iterator endN,
                             double& value, Eigen::VectorXd& grad)
{
    int nPatterns = endN - startN;
    value = error(startN, endN).mean();

    backpropagate();
    for(int p = 0; p < P; p++)
        grad(p) = *derivatives[p];
    grad /= nPatterns;
}

void ClossNet::finishedIteration()
{
    Net::finishedIteration();
    if (false) {
        OPENANN_DEBUG << "Current Parameter";
        std::ostringstream oss;
        save(oss);
        OPENANN_DEBUG << oss.str() << std::endl;
    }
}

void ClossNet::backpropagate()
{
    // initial delta is derivation of error function
    Eigen::MatrixXd delta = clossDerivative(tempError);
    Eigen::MatrixXd *pDelta = &delta;
    int l = L;
    for(std::vector<Layer*>::reverse_iterator layer = layers.rbegin();
            layer != layers.rend(); ++layer, --l)
    {
        // Backprop of dE/dX is not required in input layer and first hidden layer
        const bool backpropToPrevious = l > 2;
        (**layer).backpropagate(pDelta, pDelta, backpropToPrevious);
    }
}

template<typename Derived>
Eigen::MatrixXd ClossNet::clossFunction(const Eigen::MatrixBase<Derived> &YmT)
{
    double lambda = -1 / (2 * kernelSize * kernelSize);
    double beta = 1 / (1 - exp(lambda));
    Eigen::MatrixXd rbf = (YmT.array().abs().pow(pValue) * lambda).exp();

    auto err = (beta * (1 - rbf.array())).eval();
    return err;
}

template<typename Derived>
Eigen::MatrixXd ClossNet::clossDerivative(const Eigen::MatrixBase<Derived>& x)
{
    double lambda = -1 / (2 * kernelSize * kernelSize);
    double beta = 1 / (1 - exp(lambda));
    Eigen::MatrixXd rbf = (x.array().abs().pow(pValue) * lambda).exp();

    auto sign = [](double x) {
        return x >= 0 ? 1 : -1;
    };
    auto signx = x.unaryExpr(sign).array().eval();

    auto d = beta
             * (-lambda)
             * pValue
             * rbf.array()
             * x.array().abs().pow(pValue -1)
             * signx;

    return d.eval();
}
