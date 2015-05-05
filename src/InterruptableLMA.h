#ifndef INTERRUPTABLELMA_H
#define INTERRUPTABLELMA_H

#include <OpenANN/optimization/Optimizer.h>
#include <OpenANN/optimization/StoppingCriteria.h>
#include <Eigen/Core>
#include <optimization.h>

using OpenANN::Optimizer;
using OpenANN::Optimizable;
using OpenANN::StoppingCriteria;

class InterruptableLMA : public Optimizer
{
    StoppingCriteria stop;
    Optimizable* opt; // do not delete
    Eigen::VectorXd optimum;
    int iteration, n;
    alglib_impl::ae_state envState;
    Eigen::VectorXd parameters, errorValues, gradient;
    alglib::real_1d_array xIn;
    alglib::minlmstate state;
public:
    InterruptableLMA();
    virtual ~InterruptableLMA();
    virtual void setOptimizable(Optimizable& opt);
    virtual void setStopCriteria(const StoppingCriteria& stop);
    virtual void optimize();
    virtual bool step();
    virtual Eigen::VectorXd result();
    virtual std::string name();

    int currentIteration() const;
    double currentError() const;
protected:
    void initialize();
    void reset();
};

#endif // INTERRUPTABLELMA_H
