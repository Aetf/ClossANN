#ifndef CLOSSNET_H_
#define CLOSSNET_H_

#include <OpenANN/Net.h>
#include <Eigen/Core>
#include <vector>
#include <sstream>

using OpenANN::ActivationFunction;
/**
 * @class ClossNet
 *
 * Feedforward multilayer neural network using Closs error function.
 *
 * You can specify many different types of layers and choose the architecture
 * almost arbitrary.
 */
class ClossNet : public OpenANN::Net
{
protected:
    double kernelSize;
    double pValue;

public:
    /**
     * Create feedforward neural network.
     */
    ClossNet();

    /**
     * @name Architecture Definition
     * These functions must be called to define the architecture of the network.
     */
    ///@{
    /**
     * Add a fully connected hidden layer.
     * @param units number of nodes (neurons)
       * @param act activation function
       * @param stdDev standard deviation of the Gaussian distributed initial weights
       * @param bias add bias term
       * @return this for chaining
       */
    ClossNet& bpLayer(int units, ActivationFunction act,
                      double stdDev = 0.05, bool bias = true);
    ///@}

    /**
     * @name Persistence
     */
    ///@{
    /**
     * Save network.
     * @param stream output stream
     */
    void save(std::ostream& stream);
    /**
     * Load network from stream.
     *
     * @note Note that we cannot ensure that the network will be reconstructed
     *       correctly in case it contains either an extreme layer, compressed
     *       layer or a compressed output layer because these types of layers
     *       internally generate random matrices that will not be stored. To
     *       ensure that these matrices will contain the same values, you will
     *       have to set the seed for the random number generator, e.g.
    \code
    OpenANN::RandomNumberGenerator().seed(0);
    Net net;
    // Construct and train network
    net.save("mlnn.net");
    OpenANN::RandomNumberGenerator().seed(0);
    Net net2;
    net2.load("mlnn.net");
    \endcode
     *
     * @param stream input stream
     */
    void load(std::istream& stream);
    ///@}

    /**
     * @name Optimization Contol
     */
    ///@{
    /**
     * Disabled
     * @param l1Penalty
     * @param l2Penalty
     * @param maxSquaredWeightNorm
     * @return
     */
    Net& setRegularization(double l1Penalty = 0.0, double l2Penalty = 0.0,
                           double maxSquaredWeightNorm = 0.0);
    /**
     * Disabled
     * @param errorFunction error function
     * @return this for chaining
     */
    Net& setErrorFunction(OpenANN::ErrorFunction errorFunction);
    /**
     * Disabled
     * @param activate turn dropout on or off
     * @return this for chaining
     */
    Net& useDropout(bool activate = true);
    /**
     * Request kernelSize parameter for Closs func.
     * @return kernel size for Closs function in this neural network.
     */
    double getKernelSize() const;
    /**
     * Set kernelSize parameter for Closs func.
     * @param kernel size for Closs function in this neural network.
     * @return this for chaining
     */
    ClossNet& setKernelSize(double kernel);
    /**
     * Request pValue parameter for Closs func.
     * @return pValue for Closs function in this neural network.
     */
    double getPValue() const;
    /**
     * Set pValue parameter for Closs func.
     * @param value for p value for Closs function in this neural network.
     * @return this for chaining
     */
    ClossNet& setPValue(double value);
    ///@}

    /**
     * @name Inherited Functions
     */
    ///@{
    virtual double error(unsigned int n);
    virtual double error();
    virtual bool providesGradient();
    virtual void errorGradient(std::vector<int>::const_iterator startN,
                               std::vector<int>::const_iterator endN,
                               double& value, Eigen::VectorXd& grad);
    virtual void finishedIteration();
    ///@}

protected:
    void backpropagate();
    void forwardPropagate(double *error);
    Eigen::VectorXd error(std::vector<int>::const_iterator startN,
                          std::vector<int>::const_iterator endN);

    template<typename Derived>
    Eigen::MatrixXd clossFunction(const Eigen::MatrixBase<Derived>& ymt);
    template<typename Derived>
    Eigen::MatrixXd clossDerivative(const Eigen::MatrixBase<Derived>& x);
};


#endif // CLOSSNET_H_

