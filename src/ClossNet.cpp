#include <OpenANN/OpenANN>
#include <OpenANN/io/Logger.h>
#include <OpenANN/io/DirectStorageDataSet.h>
#include <OpenANN/util/OpenANNException.h>
#include <OpenANN/util/Random.h>
#include <Eigen/Core>
#include <iostream>

#include "ClossNet.h"

using namespace OpenANN;

/**
 * Create feedforward neural network.
 */
ClossNet::ClossNet()
    : kernelSize(0.5)
{
    errorFunction = NO_E_DEFINED;
    Net::useDropout(false);
}

void ClossNet::save(std::ostream& stream)
{
    Net::save(stream);
    stream << "kernelSize " << kernelSize;
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


double ClossNet::error(unsigned int n)
{
    tempInput = trainSet->getInstance(n).transpose();
    forwardPropagate(nullptr);

    return clossFunction(tempOutput - trainSet->getTarget(n).transpose());
}

template<typename Derived>
double ClossNet::clossFunction(const Eigen::MatrixBase<Derived> &YmT)
{
    double tmp = -2 * kernelSize * kernelSize;
    double beta = 1 / (1 - exp(1/tmp));
    double rbf = (YmT.array().square() / tmp).exp().sum() / (double) YmT.size();
    double err = beta * (1 - rbf);
    return err;

}

double ClossNet::error()
{
    double e = 0.0;
    for(int n = 0; n < N; n++)
        e += error(n) / (double) N;
    return e;
}

void ClossNet::errorGradient(std::vector<int>::const_iterator startN,
                             std::vector<int>::const_iterator endN,
                             double& value, Eigen::VectorXd& grad)
{
    const int N = endN - startN;
    tempInput.conservativeResize(N, trainSet->inputs());
    Eigen::MatrixXd T(N, trainSet->outputs());
    int n = 0;
    for(std::vector<int>::const_iterator it = startN; it != endN; ++it, ++n)
    {
        tempInput.row(n) = trainSet->getInstance(*it);
        T.row(n) = trainSet->getTarget(*it);
    }

    value = 0;
    forwardPropagate(&value);
    tempError = tempOutput - T;
    value += clossFunction(tempError);
    backpropagate();

    for(int p = 0; p < P; p++)
        grad(p) = *derivatives[p];
    grad /= N;
}
