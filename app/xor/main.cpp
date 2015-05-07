#include <OpenANN/OpenANN>
#include <OpenANN/io/Logger.h>
#include <OpenANN/io/DirectStorageDataSet.h>
#include <OpenANN/util/Random.h>
#include <Eigen/Core>
#include <iostream>

#include "ClossNet.h"

using namespace OpenANN;

int main()
{
    // Create dataset
    const int nI = 2; // number of inputs
    const int nO = 1; // number of outputs
    const int nP = 4; // size of training set
    Eigen::MatrixXd P(nP, nI); // inputs
    Eigen::MatrixXd T(nP, nO); // desired outputs (targets)
    // Each row represents an instance
    P.row(0) << 0.0, 1.0;
    T.row(0) << 1.0;
    P.row(1) << 0.0, 0.0;
    T.row(1) << 0.0;
    P.row(2) << 1.0, 1.0;
    T.row(2) << 0.0;
    P.row(3) << 1.0, 0.0;
    T.row(3) << 1.0;
    DirectStorageDataSet dataSet(&P, &T);

    // Make the result repeatable
    RandomNumberGenerator().seed(0);

    // Create network
    ClossNet net;
    // Add an input layer with nI inputs, 1 hidden layer with 2 nodes and an
    // output layer with nO outputs. Use logistic activation function in hidden
    // layer and output layer.
    makeMLNN(net, LOGISTIC, LOGISTIC, nI, nO, 1, 2);
    // Add training set
    net.trainingSet(dataSet);

    // Set stopping conditions
    StoppingCriteria stop;
    stop.minimalValueDifferences = 1e-10;
    // Train network, i.e. minimize sum of squared errors (SSE) with
    // Levenberg-Marquardt optimization algorithm until the stopping criteria
    // are satisfied.
    train(net, "LMA", MSE, stop);

    // Use network to predict labels of the training data
    for(int n = 0; n < nP; n++)
    {
        auto input = dataSet.getInstance(n);
        std::cout << input(0) << " xor " << input(1) << " = ";
        Eigen::VectorXd y = net(input);
        std::cout << y(0) << std::endl;
    }

    return 0;
}
